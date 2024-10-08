//BleskOS

/*
* MIT License
* Copyright (c) 2023-2024 Vendelín Slezák
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

void ethernet_card_realtek_8169_outb(dword_t port, byte_t value);
void ethernet_card_realtek_8169_outw(dword_t port, word_t value);
void ethernet_card_realtek_8169_outd(dword_t port, dword_t value);
byte_t ethernet_card_realtek_8169_inb(dword_t port);
word_t ethernet_card_realtek_8169_inw(dword_t port);
dword_t ethernet_card_realtek_8169_ind(dword_t port);
void initalize_ethernet_card_realtek_8169(void);
byte_t ethernet_card_realtek_8169_get_cable_status(void);
void ethernet_card_realtek_8169_send_packet(dword_t memory, dword_t length);
void ethernet_card_realtek_8169_irq(void);