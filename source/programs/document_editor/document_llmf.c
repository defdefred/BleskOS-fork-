//BleskOS

/*
* MIT License
* Copyright (c) 2023-2024 Vendelín Slezák
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 Document Low Level Memory Format

 This format consists of chars with computed final position on page. It is last layer between document and output on screen.

 First 1000 entries of 8 bytes are for pages. First dword defines width of page and second height of page.

 Every char have this format:
 dword = char number
 dword = column
 dword = line
 3 bytes = RGB color of char
 byte = char size
 3 bytes = RGB color of char background
 7 bits = emphasis
 1 bit = 0 - tranparent background 1 - color background

 Char number DLLMF_PAGE_CONTENT_END mean this is end of page. Immediately after it continues content of next page.
 Char number DLLMF_DOCUMENT_CONTENT_END mean this is end of document.
*/

/*
 Example document with char 'A' printed on first page

 ;page entries
 dd 500
 dd 800
 times 999 dq 0

 ;document data
 dd 'A'
 dd 20
 dd 20
 dd (10<<24) | (0x000000)
 dd (0x00<<24) | (0x000000)

 ;end of document
 dd DLLMF_DOCUMENT_CONTENT_END
*/

void initalize_dllmf(void) {
 dllmf_screen_first_column = 0;
 dllmf_screen_first_line = PROGRAM_INTERFACE_TOP_LINE_HEIGTH;
 dllmf_draw_width = graphic_screen_x;
 dllmf_draw_height = graphic_screen_y-PROGRAM_INTERFACE_TOP_LINE_HEIGTH-PROGRAM_INTERFACE_BOTTOM_LINE_HEIGTH;
 dllmf_draw_first_line = 0;
 dllmf_draw_last_line = (dllmf_draw_first_line+dllmf_draw_height);
 dllmf_draw_first_column = 0;
 dllmf_draw_last_column = (dllmf_draw_first_column+dllmf_draw_width);
}

void draw_dllmf(dword_t dllmf_mem) {
 dword_t *page_entries = (dword_t *) (dllmf_mem);
 dword_t *document_data = (dword_t *) (dllmf_mem+DLLMF_NUM_OF_PAGE_ENTRIES*DLLMF_PAGE_ENTRY_SIZE);
 byte_t *document_data8 = (byte_t *) (dllmf_mem+DLLMF_NUM_OF_PAGE_ENTRIES*DLLMF_PAGE_ENTRY_SIZE);
 dword_t page_first_column = 0, page_first_line = 0, page_screen_width = 0, page_screen_height = 0;

 //draw pages
 for(dword_t i=0; i<DLLMF_NUM_OF_PAGE_ENTRIES; i++, page_entries+=2) {
  if(*page_entries==0) { //no more pages
   break;
  }

  //center page
  if(page_entries[DLLMF_PAGE_ENTRY_WIDTH_OFFSET]<dllmf_draw_width) {
   page_first_column = (dllmf_screen_first_column+(dllmf_draw_width/2)-(page_entries[DLLMF_PAGE_ENTRY_WIDTH_OFFSET]/2));
  }
  else {
   page_first_column = dllmf_screen_first_column;
  }

  //draw page background
  dllmf_calculate_draw_square(page_first_column, page_first_line, page_entries[DLLMF_PAGE_ENTRY_WIDTH_OFFSET], page_entries[DLLMF_PAGE_ENTRY_HEIGHT_OFFSET]);
  draw_full_square(dllmf_square_x, dllmf_square_y, dllmf_square_width, dllmf_square_height, WHITE);

  //draw page content
  while(*document_data!=DLLMF_PAGE_CONTENT_END && *document_data!=DLLMF_DOCUMENT_CONTENT_END) {
   //draw char
   set_scalable_char_size(document_data8[DLLMF_CHAR_ENTRY_SIZE_OFFSET]);
   scalable_font_char_emphasis = (document_data8[DLLMF_CHAR_ENTRY_EMPHASIS_OFFSET] & 0x7F);
   dllmf_calculate_draw_square(page_first_column+document_data[DLLMF_CHAR_ENTRY_COLUMN_OFFSET], page_first_line+document_data[DLLMF_CHAR_ENTRY_LINE_OFFSET], document_data8[DLLMF_CHAR_ENTRY_SIZE_OFFSET], document_data8[DLLMF_CHAR_ENTRY_SIZE_OFFSET]);
   if((document_data8[DLLMF_CHAR_ENTRY_EMPHASIS_OFFSET] & 0x80)==0x80) {
    draw_full_square(dllmf_square_x, dllmf_square_y, dllmf_square_width+1, dllmf_square_height+1, document_data[DLLMF_CHAR_ENTRY_BACKGROUND_COLOR_OFFSET]);
   }
   if(dllmf_square_width==document_data8[DLLMF_CHAR_ENTRY_SIZE_OFFSET] && dllmf_square_height==document_data8[DLLMF_CHAR_ENTRY_SIZE_OFFSET]) {
    draw_scalable_char(document_data[DLLMF_CHAR_ENTRY_CHAR_NUMBER_OFFSET], dllmf_square_x, dllmf_square_y, (document_data[DLLMF_CHAR_ENTRY_COLOR_OFFSET] & 0xFFFFFF));
   }
   else {
    draw_part_of_scalable_char(document_data[DLLMF_CHAR_ENTRY_CHAR_NUMBER_OFFSET], dllmf_square_x, dllmf_square_y, dllmf_square_draw_column, dllmf_square_draw_line, dllmf_square_width, dllmf_square_height, (document_data[DLLMF_CHAR_ENTRY_COLOR_OFFSET] & 0xFFFFFF));
   }

   //move to next char
   document_data+=(DLLMF_CHAR_ENTRY_LENGTH_IN_BYTES/4);
   document_data8+=DLLMF_CHAR_ENTRY_LENGTH_IN_BYTES;
  }

  //move variable value to next page with free space between
  page_first_line+=(page_entries[DLLMF_PAGE_ENTRY_HEIGHT_OFFSET]+20);  
 }
}

void dllmf_calculate_draw_square(dword_t column, dword_t line, dword_t width, dword_t height) {
 dllmf_square_x = 0;
 dllmf_square_y = 0;
 dllmf_square_width = 0;
 dllmf_square_height = 0;
 dllmf_square_draw_column = 0;
 dllmf_square_draw_line = 0;
 
 // test if something of square is on screen
 if(line>dllmf_draw_last_line) {
  return; //not in screen
 }
 else if((line+height)<dllmf_draw_first_line) {
  return; //not in screen
 }
 if(column>dllmf_draw_last_column) {
  return; //not in screen
 }
 else if((column+width)<dllmf_draw_first_column) {
  return; //not in screen
 }
 
 //calculate Y related variables
 if(line>dllmf_draw_first_line) {
  dllmf_square_y = dllmf_screen_first_line+(line-dllmf_draw_first_line);
  dllmf_square_draw_line = 0;
  if((line+height)<=dllmf_draw_last_line) {
   dllmf_square_height = height;
  }
  else {
   dllmf_square_height = (height-(line+height-dllmf_draw_last_line));
  }
 }
 else {
  dllmf_square_y = dllmf_screen_first_line;
  dllmf_square_draw_line = (height-(line+height-dllmf_draw_first_line));
  if((height-dllmf_square_draw_line)>=internet_browser_webpage_height) {
   dllmf_square_height = internet_browser_webpage_height;
  }
  else {
   dllmf_square_height = (line+height-dllmf_draw_first_line);
  }
 }
 
 //calculate X realted variables
 if(column>dllmf_draw_first_column) {
  dllmf_square_x = (column-dllmf_draw_first_column);
  dllmf_square_draw_column = 0;
  if((column+width)<=dllmf_draw_last_column) {
   dllmf_square_width = width;
  }
  else {
   dllmf_square_width = (width-(column+width-dllmf_draw_last_column));
  }
 }
 else {
  dllmf_square_x = 0;
  dllmf_square_width = (column+width-dllmf_draw_first_column);
  dllmf_square_draw_column = (width-dllmf_square_width);
 }
}
