//BleskOS

/*
* MIT License
* Copyright (c) 2023 Vendelín Slezák
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

void initalize_image_operations(void) {
 image_resize_width_array_mem = malloc(4096);
 image_resize_heigth_array_mem = malloc(4096);
}

void copy_raw_image_data(dword_t source_memory, dword_t source_width, dword_t source_x, dword_t source_y, dword_t image_width, dword_t image_heigth, dword_t dest_memory, dword_t dest_width, dword_t dest_x, dword_t dest_y) {
 dword_t *source = (dword_t *) (source_memory + (source_y*source_width*4) + (source_x<<2));
 dword_t source_bytes_to_next_line = (source_width-image_width);
 dword_t *destination = (dword_t *) (dest_memory + (dest_y*dest_width*4) + (dest_x<<2));
 dword_t destination_bytes_to_next_line = (dest_width-image_width);
 
 for(int line=0; line<image_heigth; line++) {
  for(int column=0; column<image_width; column++) {
   if(*source!=TRANSPARENT_COLOR) {
    *destination = *source;
   }
   destination++;
   source++;
  }
  
  destination += destination_bytes_to_next_line;
  source += source_bytes_to_next_line;
 }
}

void copy_and_resize_raw_image_data(dword_t source_memory, dword_t source_width, dword_t source_heigth, dword_t resized_source_width, dword_t resized_source_heigth, dword_t source_x, dword_t source_y, dword_t image_width, dword_t image_heigth, dword_t dest_memory, dword_t dest_width, dword_t dest_x, dword_t dest_y) { 
 byte_t *image_resize_width_array = (byte_t *) image_resize_width_array_mem;
 byte_t *image_resize_heigth_array = (byte_t *) image_resize_heigth_array_mem;
 dword_t *source = (dword_t *) (source_memory);
 dword_t *destination = (dword_t *) (dest_memory + (dest_y*dest_width*4) + (dest_x<<2));
 dword_t destination_start_of_line = ((dword_t)destination);
 dword_t source_start_of_line = ((dword_t)source);
 
 //we will calculate how many times will be displayed pixel in every column(image_resize_width_array) and in every line(image_resize_heigth_array)
 //for example if we have image with size 4x4, and we want to resize it to 2x2, arrays will look like this:
 //image_resize_width_array = 0, 1, 0, 1 image_resize_heigth_array = 0, 1, 0, 1
 //if we want to resize it to size 8x4, arrays will look like this:
 //image_resize_width_array = 2, 2, 2, 2 image_resize_heigth_array = 1, 1, 1, 1
 
 //add base pixels
 dword_t width_base = (resized_source_width/source_width); 
 dword_t heigth_base = (resized_source_heigth/source_heigth);
 for(int i=0; i<4096; i++) {
  image_resize_width_array[i] = width_base;
  image_resize_heigth_array[i] = heigth_base;
 }
 
 //add all other pixels
 width_base = (resized_source_width%source_width);
 if(width_base!=0) {
  width_base++;
  
  for(int i=0, j=width_base, k=source_width; i<source_width; i++, j+=width_base) {
   if(j>k) {
    k+=source_width;
    image_resize_width_array[i]++;
   }
  }
 }
 
 heigth_base = (resized_source_heigth%source_heigth);
 if(heigth_base!=0) {
  heigth_base++;
  
  for(int i=0, j=heigth_base, k=source_heigth; i<source_heigth; i++, j+=heigth_base) {
   if(j>k) {
    k+=source_heigth;
    image_resize_heigth_array[i]++;
   }
  }
 }
 
 //edit arrays to drawed image size
 for(int i=0; i<4096; i++) {
  if(source_x>0) {
   if(image_resize_width_array[i]>source_x) {
    image_resize_width_array[i]-=source_x;
    source_x=0;
    
    if(image_resize_width_array[i]>image_width) {
     image_resize_width_array[i]=image_width;
     image_width=0;
    }
    else {
     image_width-=image_resize_width_array[i];
    }
   }
   else {
    source_x-=image_resize_width_array[i];
    image_resize_width_array[i]=0;
   }
  }
  else if(image_width>0) {
   if(image_resize_width_array[i]>image_width) {
    image_resize_width_array[i]=image_width;
    image_width=0;
   }
   else {
    image_width-=image_resize_width_array[i];
   }
  }
  else {
   image_resize_width_array[i]=0;
  }
 }
 
 for(int i=0; i<4096; i++) {
  if(source_y>0) {
   if(image_resize_heigth_array[i]>source_y) {
    image_resize_heigth_array[i]-=source_y;
    source_y=0;
    
    if(image_resize_heigth_array[i]>image_heigth) {
     image_resize_heigth_array[i]=image_heigth;
     image_heigth=0;
    }
    else {
     image_heigth-=image_resize_heigth_array[i];
    }
   }
   else {
    source_y-=image_resize_heigth_array[i];
    image_resize_heigth_array[i]=0;
   }
  }
  else if(image_heigth>0) {
   if(image_resize_heigth_array[i]>image_heigth) {
    image_resize_heigth_array[i]=image_heigth;
    image_heigth=0;
   }
   else {
    image_heigth-=image_resize_heigth_array[i];
   }
  }
  else {
   image_resize_heigth_array[i]=0;
  }
 }
 
 //copy data
 for(int line=0; line<source_heigth; line++) {
  for(int pixel_heigth=0; pixel_heigth<image_resize_heigth_array[line]; pixel_heigth++) {
   source = (dword_t *) source_start_of_line;
   destination = (dword_t *) destination_start_of_line;
  
   for(int column=0; column<source_width; column++) {   
    for(int pixel_width=0; pixel_width<image_resize_width_array[column]; pixel_width++) {
     if(*source!=TRANSPARENT_COLOR) {
      *destination = *source;
     }
     destination++;
    }
    source++;
   }
  
   destination_start_of_line += (dest_width*4);
  }

  source_start_of_line += (source_width*4);
 }
}

dword_t create_image(dword_t width, dword_t heigth) {
 dword_t image_info_mem = calloc(IMAGE_SIZE_OF_INFO_IN_BYTES+(width*heigth*4));
 dword_t *image_info = (dword_t *) image_info_mem;

 image_info[IMAGE_INFO_REAL_WIDTH] = width;
 image_info[IMAGE_INFO_REAL_HEIGTH] = heigth;
 image_info[IMAGE_INFO_WIDTH] = width;
 image_info[IMAGE_INFO_HEIGTH] = heigth;
 image_info[IMAGE_INFO_DRAW_X] = 0;
 image_info[IMAGE_INFO_DRAW_Y] = 0;
 image_info[IMAGE_INFO_DRAW_WIDTH] = width;
 image_info[IMAGE_INFO_DRAW_HEIGTH] = heigth;
 image_info[IMAGE_INFO_SCREEN_X] = 0;
 image_info[IMAGE_INFO_SCREEN_Y] = 0;
 image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_SIZE] = 0;
 image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_POSITION] = 0;
 image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_SIZE] = 0;
 image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_POSITION] = 0;
 
 return image_info_mem;
}

void delete_image(dword_t image_info_mem) {
 free(image_info_mem);
}

dword_t get_image_data_memory(dword_t image_info_mem) {
 return image_info_mem+IMAGE_SIZE_OF_INFO_IN_BYTES;
}

void draw_image(dword_t image_info_mem) {
 dword_t *image_info = (dword_t *) image_info_mem;
 
 //draw image
 copy_raw_image_data((image_info_mem+IMAGE_SIZE_OF_INFO_IN_BYTES), image_info[IMAGE_INFO_REAL_WIDTH], image_info[IMAGE_INFO_DRAW_X], image_info[IMAGE_INFO_DRAW_Y], image_info[IMAGE_INFO_DRAW_WIDTH], image_info[IMAGE_INFO_DRAW_HEIGTH], screen_mem, graphic_screen_x, image_info[IMAGE_INFO_SCREEN_X], image_info[IMAGE_INFO_SCREEN_Y]);

 //draw scrollbars
 if(image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_SIZE]>0) {
  draw_vertical_scrollbar(image_info[IMAGE_INFO_SCREEN_X]+image_info[IMAGE_INFO_DRAW_WIDTH], image_info[IMAGE_INFO_SCREEN_Y], image_info[IMAGE_INFO_DRAW_HEIGTH], image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_POSITION], image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_SIZE]);
 }
 if(image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_SIZE]>0) {
  draw_horizontal_scrollbar(image_info[IMAGE_INFO_SCREEN_X], image_info[IMAGE_INFO_SCREEN_Y]+image_info[IMAGE_INFO_DRAW_HEIGTH], image_info[IMAGE_INFO_DRAW_WIDTH], image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_POSITION], image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_SIZE]);
 }
}

void draw_resized_image(dword_t image_info_mem) {
 dword_t *image_info = (dword_t *) image_info_mem;
 
 //draw image
 copy_and_resize_raw_image_data((image_info_mem+IMAGE_SIZE_OF_INFO_IN_BYTES), image_info[IMAGE_INFO_REAL_WIDTH], image_info[IMAGE_INFO_REAL_HEIGTH], image_info[IMAGE_INFO_WIDTH], image_info[IMAGE_INFO_HEIGTH], image_info[IMAGE_INFO_DRAW_X], image_info[IMAGE_INFO_DRAW_Y], image_info[IMAGE_INFO_DRAW_WIDTH], image_info[IMAGE_INFO_DRAW_HEIGTH], screen_mem, graphic_screen_x, image_info[IMAGE_INFO_SCREEN_X], image_info[IMAGE_INFO_SCREEN_Y]);

 //draw scrollbars
 if(image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_SIZE]>0) {
  draw_vertical_scrollbar(image_info[IMAGE_INFO_SCREEN_X]+image_info[IMAGE_INFO_DRAW_WIDTH], image_info[IMAGE_INFO_SCREEN_Y], image_info[IMAGE_INFO_DRAW_HEIGTH], image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_POSITION], image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_SIZE]);
 }
 if(image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_SIZE]>0) {
  draw_horizontal_scrollbar(image_info[IMAGE_INFO_SCREEN_X], image_info[IMAGE_INFO_SCREEN_Y]+image_info[IMAGE_INFO_DRAW_HEIGTH], image_info[IMAGE_INFO_DRAW_WIDTH], image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_POSITION], image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_SIZE]);
 }
}

void calculate_image_scrollbars(dword_t image_info_mem) {
 dword_t *image_info = (dword_t *) image_info_mem;
 
 image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_SIZE] = 0;
 image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_POSITION] = 0;
 image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_SIZE] = 0;
 image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_POSITION] = 0;

 if(image_info[IMAGE_INFO_DRAW_HEIGTH]<image_info[IMAGE_INFO_HEIGTH]) {
  dword_t rider_size = calculate_scrollbar_rider_size(image_info[IMAGE_INFO_DRAW_HEIGTH], image_info[IMAGE_INFO_HEIGTH], image_info[IMAGE_INFO_DRAW_HEIGTH]);
  image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_SIZE] = rider_size;
  image_info[IMAGE_INFO_VERTICAL_SCROLLBAR_RIDER_POSITION] = calculate_scrollbar_rider_position(image_info[IMAGE_INFO_DRAW_HEIGTH], rider_size, image_info[IMAGE_INFO_HEIGTH], image_info[IMAGE_INFO_DRAW_HEIGTH], image_info[IMAGE_INFO_DRAW_Y]);
 }
 if(image_info[IMAGE_INFO_DRAW_WIDTH]<image_info[IMAGE_INFO_WIDTH]) { 
  dword_t rider_size = calculate_scrollbar_rider_size(image_info[IMAGE_INFO_DRAW_WIDTH], image_info[IMAGE_INFO_WIDTH], image_info[IMAGE_INFO_DRAW_WIDTH]);
  image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_SIZE] = rider_size;
  image_info[IMAGE_INFO_HORIZONTAL_SCROLLBAR_RIDER_POSITION] = calculate_scrollbar_rider_position(image_info[IMAGE_INFO_DRAW_WIDTH], rider_size, image_info[IMAGE_INFO_WIDTH], image_info[IMAGE_INFO_DRAW_WIDTH], image_info[IMAGE_INFO_DRAW_X]);
 }
}

void get_mouse_coordinates_on_image(dword_t image_info_mem) {
 dword_t *image_info = (dword_t *) image_info_mem;
 byte_t *image_resize_width_array = (byte_t *) image_resize_width_array_mem;
 byte_t *image_resize_heigth_array = (byte_t *) image_resize_heigth_array_mem;
 
 image_mouse_x = 0xFFFFFFFF;
 image_mouse_y = 0xFFFFFFFF;
 if(image_info[IMAGE_INFO_SCREEN_X]>mouse_cursor_x) {
  return;
 }
 if(image_info[IMAGE_INFO_SCREEN_Y]>mouse_cursor_y) {
  return;
 }
 
 dword_t resized_source_width = image_info[IMAGE_INFO_WIDTH];
 dword_t resized_source_heigth = image_info[IMAGE_INFO_HEIGTH];
 dword_t source_width = image_info[IMAGE_INFO_REAL_WIDTH];
 dword_t source_heigth = image_info[IMAGE_INFO_REAL_HEIGTH];
 dword_t source_x = image_info[IMAGE_INFO_DRAW_X];
 dword_t source_y = image_info[IMAGE_INFO_DRAW_Y];
 dword_t image_width = image_info[IMAGE_INFO_DRAW_WIDTH];
 dword_t image_heigth = image_info[IMAGE_INFO_DRAW_HEIGTH];
 
 //add base pixels
 dword_t width_base = (resized_source_width/source_width); 
 dword_t heigth_base = (resized_source_heigth/source_heigth);
 for(int i=0; i<4096; i++) {
  image_resize_width_array[i] = width_base;
  image_resize_heigth_array[i] = heigth_base;
 }
 
 //add all other pixels
 width_base = (resized_source_width%source_width);
 if(width_base!=0) {
  width_base++;
  
  for(int i=0, j=width_base, k=source_width; i<source_width; i++, j+=width_base) {
   if(j>k) {
    k+=source_width;
    image_resize_width_array[i]++;
   }
  }
 }
 
 heigth_base = (resized_source_heigth%source_heigth);
 if(heigth_base!=0) {
  heigth_base++;
  
  for(int i=0, j=heigth_base, k=source_heigth; i<source_heigth; i++, j+=heigth_base) {
   if(j>k) {
    k+=source_heigth;
    image_resize_heigth_array[i]++;
   }
  }
 }
 
 //edit arrays to drawed image size
 for(int i=0; i<4096; i++) {
  if(source_x>0) {
   if(image_resize_width_array[i]>source_x) {
    image_resize_width_array[i]-=source_x;
    source_x=0;
    
    if(image_resize_width_array[i]>image_width) {
     image_resize_width_array[i]=image_width;
     image_width=0;
    }
    else {
     image_width-=image_resize_width_array[i];
    }
   }
   else {
    source_x-=image_resize_width_array[i];
    image_resize_width_array[i]=0;
   }
  }
  else if(image_width>0) {
   if(image_resize_width_array[i]>image_width) {
    image_resize_width_array[i]=image_width;
    image_width=0;
   }
   else {
    image_width-=image_resize_width_array[i];
   }
  }
  else {
   image_resize_width_array[i]=0;
  }
 }
 
 for(int i=0; i<4096; i++) {
  if(source_y>0) {
   if(image_resize_heigth_array[i]>source_y) {
    image_resize_heigth_array[i]-=source_y;
    source_y=0;
    
    if(image_resize_heigth_array[i]>image_heigth) {
     image_resize_heigth_array[i]=image_heigth;
     image_heigth=0;
    }
    else {
     image_heigth-=image_resize_heigth_array[i];
    }
   }
   else {
    source_y-=image_resize_heigth_array[i];
    image_resize_heigth_array[i]=0;
   }
  }
  else if(image_heigth>0) {
   if(image_resize_heigth_array[i]>image_heigth) {
    image_resize_heigth_array[i]=image_heigth;
    image_heigth=0;
   }
   else {
    image_heigth-=image_resize_heigth_array[i];
   }
  }
  else {
   image_resize_heigth_array[i]=0;
  }
 }
 
 //find coordinates
 for(int i=0, j=0, mouse_cursor=(mouse_cursor_x-image_info[IMAGE_INFO_SCREEN_X]); i<4096; i++) {
  j+=image_resize_width_array[i];
  
  if(j>mouse_cursor) {
   image_mouse_x=i;
   break;
  }
 }
 for(int i=0, j=0, mouse_cursor=(mouse_cursor_y-image_info[IMAGE_INFO_SCREEN_Y]); i<4096; i++) {
  j+=image_resize_heigth_array[i];
  
  if(j>mouse_cursor) {
   image_mouse_y=i;
   break;
  }
 }
}

void image_reverse_horizontally(dword_t image_info_mem) {
 dword_t *image_info = (dword_t *) image_info_mem;
 dword_t image_data_1_starting_point = (image_info_mem+IMAGE_SIZE_OF_INFO_IN_BYTES);
 dword_t *image_data_1 = (dword_t *) image_data_1_starting_point;
 dword_t image_data_2_starting_point = (image_info_mem+IMAGE_SIZE_OF_INFO_IN_BYTES+(image_info[IMAGE_INFO_REAL_WIDTH]*4)-4);
 dword_t *image_data_2 = (dword_t *) image_data_2_starting_point;
 dword_t color = 0;
 
 for(int line=0; line<image_info[IMAGE_INFO_REAL_HEIGTH]; line++) {
  image_data_1 = (dword_t *) image_data_1_starting_point;
  image_data_2 = (dword_t *) image_data_2_starting_point;
  
  for(int column=0; column<(image_info[IMAGE_INFO_REAL_WIDTH]/2); column++) {
   color = *image_data_1;
   *image_data_1 = *image_data_2;
   *image_data_2 = color;
   image_data_1++;
   image_data_2--;
  }
  
  image_data_1_starting_point += (image_info[IMAGE_INFO_REAL_WIDTH]*4);
  image_data_2_starting_point += (image_info[IMAGE_INFO_REAL_WIDTH]*4);
 }
}

void image_reverse_vertically(dword_t image_info_mem) {
 dword_t *image_info = (dword_t *) image_info_mem;
 dword_t image_data_1_starting_point = (image_info_mem+IMAGE_SIZE_OF_INFO_IN_BYTES);
 dword_t *image_data_1 = (dword_t *) image_data_1_starting_point;
 dword_t image_data_2_starting_point = (image_info_mem+IMAGE_SIZE_OF_INFO_IN_BYTES+(image_info[IMAGE_INFO_REAL_WIDTH]*image_info[IMAGE_INFO_REAL_HEIGTH]*4)-(image_info[IMAGE_INFO_REAL_WIDTH]*4));
 dword_t *image_data_2 = (dword_t *) image_data_2_starting_point;
 dword_t color = 0;
 
 for(int line=0; line<(image_info[IMAGE_INFO_REAL_HEIGTH]/2); line++) {
  image_data_1 = (dword_t *) image_data_1_starting_point;
  image_data_2 = (dword_t *) image_data_2_starting_point;
  
  for(int column=0; column<image_info[IMAGE_INFO_REAL_WIDTH]; column++) {
   color = *image_data_1;
   *image_data_1 = *image_data_2;
   *image_data_2 = color;
   image_data_1++;
   image_data_2++;
  }
  
  image_data_1_starting_point += (image_info[IMAGE_INFO_REAL_WIDTH]*4);
  image_data_2_starting_point -= (image_info[IMAGE_INFO_REAL_WIDTH]*4);
 }
}

void image_turn_left(dword_t image_info_mem) {
 dword_t *image_info = (dword_t *) image_info_mem;
 dword_t *image_data = (dword_t *) (image_info_mem+IMAGE_SIZE_OF_INFO_IN_BYTES);
 dword_t second_image_mem = malloc(image_info[IMAGE_INFO_REAL_WIDTH]*image_info[IMAGE_INFO_REAL_HEIGTH]*4);
 dword_t second_image_data_starting_point = (second_image_mem+(image_info[IMAGE_INFO_REAL_WIDTH]*image_info[IMAGE_INFO_REAL_HEIGTH]*4)-(image_info[IMAGE_INFO_REAL_HEIGTH]*4));
 dword_t *second_image_data = (dword_t *) second_image_data_starting_point;
 
 //turn image
 for(int line=0; line<image_info[IMAGE_INFO_REAL_HEIGTH]; line++) {
  second_image_data = (dword_t *) second_image_data_starting_point;
  
  for(int column=0; column<image_info[IMAGE_INFO_REAL_WIDTH]; column++) {
   *second_image_data = *image_data;
   image_data++;
   second_image_data -= image_info[IMAGE_INFO_REAL_HEIGTH];
  }
  
  second_image_data_starting_point+=4;
 }
 
 //copy result
 image_data = (dword_t *) (image_info_mem+IMAGE_SIZE_OF_INFO_IN_BYTES);
 second_image_data = (dword_t *) second_image_mem;
 for(int i=0; i<(image_info[IMAGE_INFO_REAL_WIDTH]*image_info[IMAGE_INFO_REAL_HEIGTH]); i++) {
  *image_data = *second_image_data;
  image_data++;
  second_image_data++;
 }
 
 //finalize turn
 dword_t var=image_info[IMAGE_INFO_REAL_HEIGTH];
 image_info[IMAGE_INFO_REAL_HEIGTH] = image_info[IMAGE_INFO_REAL_WIDTH];
 image_info[IMAGE_INFO_REAL_WIDTH] = var;
 free(second_image_mem);
}

void image_turn_right(dword_t image_info_mem) {
 dword_t *image_info = (dword_t *) image_info_mem;
 dword_t *image_data = (dword_t *) (image_info_mem+IMAGE_SIZE_OF_INFO_IN_BYTES+(image_info[IMAGE_INFO_REAL_WIDTH]*image_info[IMAGE_INFO_REAL_HEIGTH]*4)-4);
 dword_t second_image_mem = malloc(image_info[IMAGE_INFO_REAL_WIDTH]*image_info[IMAGE_INFO_REAL_HEIGTH]*4);
 dword_t second_image_data_starting_point = (second_image_mem+(image_info[IMAGE_INFO_REAL_WIDTH]*image_info[IMAGE_INFO_REAL_HEIGTH]*4)-(image_info[IMAGE_INFO_REAL_HEIGTH]*4));
 dword_t *second_image_data = (dword_t *) second_image_data_starting_point;
 
 //turn image
 for(int line=0; line<image_info[IMAGE_INFO_REAL_HEIGTH]; line++) {
  second_image_data = (dword_t *) second_image_data_starting_point;
  
  for(int column=0; column<image_info[IMAGE_INFO_REAL_WIDTH]; column++) {
   *second_image_data = *image_data;
   image_data--;
   second_image_data -= image_info[IMAGE_INFO_REAL_HEIGTH];
  }
  
  second_image_data_starting_point+=4;
 }
 
 //copy result
 image_data = (dword_t *) (image_info_mem+IMAGE_SIZE_OF_INFO_IN_BYTES);
 second_image_data = (dword_t *) second_image_mem;
 for(int i=0; i<(image_info[IMAGE_INFO_REAL_WIDTH]*image_info[IMAGE_INFO_REAL_HEIGTH]); i++) {
  *image_data = *second_image_data;
  image_data++;
  second_image_data++;
 }
 
 //finalize turn
 dword_t var=image_info[IMAGE_INFO_REAL_HEIGTH];
 image_info[IMAGE_INFO_REAL_HEIGTH] = image_info[IMAGE_INFO_REAL_WIDTH];
 image_info[IMAGE_INFO_REAL_WIDTH] = var;
 free(second_image_mem);
}