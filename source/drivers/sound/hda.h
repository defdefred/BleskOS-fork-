//BleskOS

/*
* MIT License
* Copyright (c) 2023 Vendelín Slezák
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#define HDA_CORB_RIRB 1
#define HDA_PIO 2

#define HDA_NODE_OUTPUT 0x0
#define HDA_NODE_INPUT 0x1
#define HDA_NODE_MIXER 0x2
#define HDA_NODE_SELECTOR 0x3
#define HDA_NODE_PIN 0x4

#define HDA_PIN_LINE_OUT 0x10
#define HDA_PIN_SPEAKER 0x11

byte_t hda_present = 0, hda_verb_interface, hda_playing_state=0;
dword_t hda_base = 0;
dword_t hda_input_stream_base = 0, hda_input_buffer_list = 0;
dword_t hda_output_stream_base = 0, hda_output_buffer_list = 0;
dword_t hda_corb_mem, hda_corb_pointer;
dword_t hda_rirb_mem, hda_rirb_pointer;
dword_t hda_codec_number, hda_codec_nodes_types_mem, hda_codec_nodes_connection_mem;
dword_t hda_output_pin_node, hda_node_in_path, hda_output_audio_node;
dword_t hda_sound_length, hda_output_sound_capabilites;

void initalize_hda(void);
void hda_initalize_codec(dword_t codec);
void hda_node_set_volume(dword_t codec, dword_t node, dword_t volume);
byte_t hda_is_supported_channel_size(byte_t size);
byte_t hda_is_supported_sample_rate(dword_t sample_rate);
dword_t hda_send_verb(dword_t codec, dword_t node, dword_t verb, dword_t command);
void hda_play_memory(dword_t memory, dword_t length);
void hda_stop_sound(void);
void hda_resume_sound(void);
