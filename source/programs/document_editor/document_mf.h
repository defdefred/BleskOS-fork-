//BleskOS

/*
* MIT License
* Copyright (c) 2023-2024 Vendelín Slezák
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#define DMF_SECTION_FORMAT_CHANGE_SIGNATURE 0xFFFF

#define DMF_SFCH_ENTRY_NO_CHANGE 0xFFFF
#define DMF_SFCH_ENTRY_LENGTH_IN_BYTES 23*2
#define DMF_SFCH_SIGNATURE_OFFSET_1 0
#define DMF_SFCH_LENGTH_OFFSET_1 1
#define DMF_SFCH_LENGTH_OFFSET_2 21
#define DMF_SFCH_SIGNATURE_OFFSET_2 22

#define DMF_SFCH_DESCRIPTION_OFFSET 2
 #define DMF_SFCH_DESCRIPTION_NEW_PAGE 0x1
 #define DMF_SFCH_DESCRIPTION_NEW_PARAGRAPH 0x2
#define DMF_SFCH_NEW_PAGE_WIDTH_OFFSET 3
#define DMF_SFCH_NEW_PAGE_HEIGHT_OFFSET 4
#define DMF_SFCH_NEW_PAGE_TOP_BORDER_OFFSET 5
#define DMF_SFCH_NEW_PAGE_BOTTOM_BORDER_OFFSET 6
#define DMF_SFCH_NEW_PAGE_LEFT_BORDER_OFFSET 7
#define DMF_SFCH_NEW_PAGE_RIGHT_BORDER_OFFSET 8

#define DMF_SFCH_PARAGRAPH_DESCRIPTION_OFFSET 9
 #define DMF_SFCH_PARAGRAPH_DESCRIPTION_TOP_BORDER_COLLAPSE 0x1
 #define DMF_SFCH_PARAGRAPH_DESCRIPTION_BOTTOM_BORDER_COLLAPSE 0x2
 #define DMF_SFCH_PARAGRAPH_DESCRIPTION_LIST_ENTRY 0x4
#define DMF_SFCH_PARAGRAPH_TOP_BORDER_OFFSET 10
#define DMF_SFCH_PARAGRAPH_BOTTOM_BORDER_OFFSET 11
#define DMF_SFCH_PARAGRAPH_LEFT_BORDER_OFFSET 12
#define DMF_SFCH_PARAGRAPH_RIGHT_BORDER_OFFSET 13
#define DMF_SFCH_PARAGRAPH_LIST_ENTRY_OFFSET 14
 #define DMF_SFCH_PARAGRAPH_LIST_ENTRY_DOT 0xFFFF

#define DMF_SFCH_INLINE_CHANGE_SIZE_OFFSET 15
#define DMF_SFCH_INLINE_CHANGE_EMPHASIS_OFFSET 16
#define DMF_SFCH_INLINE_CHANGE_COLOR_OFFSET 17
#define DMF_SFCH_INLINE_CHANGE_BACKGROUND_COLOR_OFFSET 19
 #define DMF_SFCH_COLOR_NO_CHANGE 0xFFFFFFFF

dword_t dmf_page_width = 0, dmf_page_height = 0, dmf_page_top_border = 0, dmf_page_bottom_border = 0, dmf_page_left_border = 0, dmf_page_right_border = 0;
dword_t dmf_page_actual_left_border = 0, dmf_page_actual_right_border = 0;
dword_t dmf_actual_paragraph_description = 0, dmf_paragraph_top_border = 0, dmf_paragraph_bottom_border = 0, dmf_paragraph_left_border = 0, dmf_paragraph_right_border = 0;
dword_t dmf_character_size = 0, dmf_character_spacing = 0, dmf_character_emphasis = 0, dmf_character_color = 0, dmf_character_background_color = 0;

void convert_dmf_to_dllmf(dword_t dmf_memory, dword_t dllmf_memory);