//BleskOS

/*
* MIT License
* Copyright (c) 2023-2025 Vendelín Slezák
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

dword_t ec_broadcom_netxtreme_read(dword_t number_of_card, dword_t port) {
 return mmio_ind(ethernet_cards[number_of_card].base+port);
}

void ec_broadcom_netxtreme_write(dword_t number_of_card, dword_t port, dword_t value) {
 mmio_outd(ethernet_cards[number_of_card].base+port, value);
}

void ec_broadcom_netxtreme_set_bits(dword_t number_of_card, dword_t port, dword_t bits) {
 mmio_outd(ethernet_cards[number_of_card].base+port, (mmio_ind(ethernet_cards[number_of_card].base+port) | bits));
}

void ec_broadcom_netxtreme_clear_bits(dword_t number_of_card, dword_t port, dword_t bits) {
 mmio_outd(ethernet_cards[number_of_card].base+port, (mmio_ind(ethernet_cards[number_of_card].base+port) & ~bits));
}

word_t ec_broadcom_netxtreme_read_phy(dword_t number_of_card, byte_t phy_register) {
 ec_broadcom_netxtreme_write(number_of_card, 0x44C, ( (1 << 29) | (0b10 << 26) | (0b1 << 21) | (phy_register << 16) | (0x0000 << 0) ));
 wait(10);
 log("\n0x404: "); log_hex(ec_broadcom_netxtreme_read(number_of_card, 0x404));
 log("\n0x6810: "); log_hex(ec_broadcom_netxtreme_read(number_of_card, 0x6810));
 log("\n0x684C: "); log_hex(ec_broadcom_netxtreme_read(number_of_card, 0x684C));
 return (ec_broadcom_netxtreme_read(number_of_card, 0x44C) & 0xFFFF);
}

void ec_broadcom_netxtreme_write_phy(dword_t number_of_card, byte_t phy_register, word_t value) {
 ec_broadcom_netxtreme_write(number_of_card, 0x44C, ( (1 << 29) | (0b01 << 26) | (0b1 << 21) | (phy_register << 16) | (value << 0) ));
 wait(10);
 log("\n0x404: "); log_hex(ec_broadcom_netxtreme_read(number_of_card, 0x404));
 log("\n0x6810: "); log_hex(ec_broadcom_netxtreme_read(number_of_card, 0x6810));
 log("\n0x684C: "); log_hex(ec_broadcom_netxtreme_read(number_of_card, 0x684C));
}

void ec_broadcom_netxtreme_initalize(dword_t number_of_card) {
 //set card methods
 ethernet_cards[number_of_card].get_cable_status = ec_broadcom_netxtreme_get_cable_status;
 ethernet_cards[number_of_card].send_packet = ec_broadcom_netxtreme_send_packet;
 ethernet_cards[number_of_card].process_irq = ec_broadcom_netxtreme_process_irq;

 //HOST coalescing
 // The number of BDs consumed for frames received, without updating receive indices on the host, is equal
 // to or has exceeded the threshold set in the Receive_Max_Coalesced_BD register (see “Receive Max
 // Coalesced BD Count Register (offset: 0x3C10)” on page 446).
 // • The number of BDs consumed for transmitting frames, without updating the send indices, on the host is
 // equal to or has exceeded the threshold set in the Send_Max_Coalesced_BD register (see “Send Max
 // Coalesced BD Count During Interrupt (offset 0x3C1C)” on page 447). Updates can occur when the number
 // of BDs (not frames) meets the thresholds set in the various coalescing registers (see Section 10: “Interrupt
 // Processing,” on page 219 for more information).
 // • The receive coalescing timer has expired, and new frames have been received on any of the receive rings,
 // and a host update has not occurred. The receive coalescing timer is then reset to the value in the
 // Receive_Coalescing_Ticks register (see “Send Coalescing Ticks Register (offset: 0x3C0C)” on page 445).
 // • The send coalescing timer has expired, and new frames have been consumed from any send ring, and a
 // host update has not occurred. The send coalescing timer is then reset to the value in the
 // Send_Coalescing_Ticks register.

 // Setup of Producer Rings Using RCBs
 // A Ring Control Block (RCB) is used by the host software to set up the shared rings in host memory. In the context
 // of producer ring, the Receive Producer Ring RCB is a register that is used to define the Receive Producer Ring.
 // The host software must initialize the Receive Producer Ring RCB.
 // Receive Producer Ring RCB — Register Offset 0x2450–0x245f
 // Other Considerations Relating to Producer Ring Setup
 // Other registers that affect the producer rings must be initialized by the host software. These registers include
 // the Receive BD Ring Replenish Threshold register, the Receive MTU register, and the Accept Oversized bit (bit
 // 5) in the Receive MAC Mode register.
 // • Receive BD Producer Ring Replenish Threshold registers:
 // – “Standard Receive BD Producer Ring Replenish Threshold Register (offset: 0x2C18)” on page 382
 // – “Receive Data Completion Control Registers” on page 381.
 // These registers are used for setting the number of BDs that the Ethernet controller can use up before
 // requesting that more BDs be DMAed from a producer ring. In other words, the device does not initiate a
 // DMA for fetching the RX BDs until the number of BDs made available to the device by the host is at least
 // the value programmed in this register.
 // • Receive MTU register (“Receive MTU Size Register (offset: 0x43C)” on page 327). This 32-bit register is set
 // to a value that is the maximum size of a packet that the Ethernet controller receives. Any packets above
 // this size is labeled as an oversized packet. The value for this register is typically set to 1518, which is the
 // Standard Producer Ring RCB typical setting. If Jumbo frames are supported, the MTU would be set to the
 // maximum Jumbo frame size.
 // • Receive MAC Mode register (“Receive MAC Mode Register (offset: 0x468)” on page 332). If the Accept
 // Oversized bit (bit 5) of this register is set, the Ethernet controller accepts packets (of size up to 64 KB)
 // larger than the size specified in the MTU.

 // 0x200 - 0x207 and 0x5800 - 0x5807 Interrupt Mailbox 0 RW
 // 0x208 - 0x20F and 0x5808 - 0x580F Interrupt Mailbox 1 RW
 // 0x268 - 0x26F and 0x5868 - 0x586F Receive BD Standard Producer Ring Producer Index RW
 // 0x280 - 0x287 and 0x5880 - 0x5887 Receive BD Return Ring 1 Consumer Index RW 
 // 0x288 - 0x28F and 0x5888 - 0x588F Receive BD Return Ring 2 Consumer Index RW
 // 0x290 - 0x297 and 0x5890 - 0x5897 Receive BD Return Ring Consumer Index RW

 //0x0100 Send Control Ring
 //“Send BD Ring Host Producer Index (High Priority Mailbox) Register (offset: 0x300-0x307)”

 // A hardware reset initiated by the PCI reset signal will initialize all PCI configuration registers and device MAC
 // registers to their default values. The driver reset via the Core Clock Blocks Reset bit (see “Miscellaneous
 // Configuration Register (offset: 0x6804)” on page 523) will also initialize all non-sticky registers to their default
 // values. The content of the device internal memory remains unchanged after warm reset (any reset with the
 // power supplied to the device).

 // The MAC address registers, starting at offset 0x0410, contain the MAC addresses of the NIC. These registers are
 // usually initialized with a default MAC address extracted from the NIC NVRAM when it is first powered up. The
 // host software may overwrite the default MAC address by writing to the MAC registers with a new MAC address.
 // Table 53 illustrates the MAC register format.
 // The BCM5725/BCM5762/BCM57767 Ethernet controller allows a NIC to have up to four MAC addresses (offset
 // 0x410–0x42F) that are used for hardware packet reception filtering

 //The host software may enable promiscuous mode by setting the Promiscuous_Mode bit (bit 8) of the Receive_MAC_Mode register (offset 0x468).

 // Initialization Procedure
 // This section describes the initialization procedure for the MAC portion of the NetXtreme family of devices.
 // 1. Enable MAC memory space decode and bus mastering. If the device has not been initialized previously
 // (power-on reset), the host software must enable these bits to be able to issue the core clock reset in Step 6.
 // Set the Bus_Master and Memory_Space bits in the PCI Configuration Space Command register (see PCI
 // “Status and Command Register (Offset: 0x04) — Function 0” on page 245).
 // 2. Disable interrupts. Set the Mask_Interrupt bit in the Miscellaneous Host Control register (see
 // “Miscellaneous Host Control Register (offset: 0x68) — Function 0” on page 256).
 // 3. Write the T3_MAGIC_NUMBER (0x4B657654 = “KevT”) to the device memory at offset 0xB50 to notify the
 // boot code that the following reset is a warm reset (driver initiated core_clocks reset).
 // 4. Acquire the NVRAM lock by setting the REQ1 bit of the Software Arbitration register (see “Software
 // Arbitration Register (Offset 0x7020)” on page 559) and then waiting for the ARB_WON1 bit to be set.
 // 5. Clear the Fast Boot Program Counter register (Offset 0x6894) and enable the Memory Arbiter as specified
 // in Step 10. Also initialize the Misc Host Control register as specified in Step 11.
 // 6. Reset the core clocks. Set the CORE_Clock_Blocks-Reset bit in the General Control Miscellaneous
 // Configuration register (see “Miscellaneous Configuration Register (offset: 0x6804)” on page 523). The
 // GPHY_Power_Down_Override bit (bit 26) and the Disable_GRC_Reset_on_PCI-E_Block bit (bit 29) should
 // also be set to 1.
 // 7. Wait for core-clock reset to complete. The core clock reset disables indirect mode and flat/standard modes.
 // Software cannot poll the core-clock reset bit to deassert since the local memory interface is disabled by the
 // reset. Driver should delay minimum of 1 millisecond at this point.
 // 8. Disable interrupts. Set the Mask_PCI_Interrupt_Output bit in the Miscellaneous Host Control register (see
 // “Miscellaneous Host Control Register (offset: 0x68) — Function 0” on page 256). The bit was reset after the
 // core_clock reset and interrupts must be masked off again.
 // 9. Enable MAC memory space decode and bus mastering. Set the Bus_Master and Memory_Space bits in the
 // PCI Configuration Space Command register (see “Status and Command Register (Offset: 0x04) — Function
 // 0” on page 245).
 // 10. Enable the MAC memory arbiter. Set the Enable bit in the Memory Arbiter Mode register (see “Memory
 // Arbiter Mode Register (offset: 0x4000)” on page 451).
 // 11. Initialize the Miscellaneous Host Control register (see “Miscellaneous Host Control Register (offset:
 // 0x68) — Function 0” on page 256).
 // a. Set Endian Word Swap (optional). When the host processor architecture is big-endian, the MAC may
 // word swap data when acting as a PCI target device. Set the Enable_Endian_Word_Swap bit in the
 // Miscellaneous Host Control register.
 // b. Set Endian Byte Swap (optional). When the host processor architecture is big-endian, the MAC may byte
 // swap data when acting as a PCI target device. Set the Enable_Endian_Byte_Swap bit in the
 // Miscellaneous Host Control register.
 // c. Enable the indirect register pairs (see “Indirect Mode” on page 164). Set the Enable_Indirect_Access bit
 // in the Miscellaneous Host Control register.
 // d. Enable the PCI State register to allow the device driver read/write access by setting the
 // Enable_PCI_State_Register bit in the Miscellaneous Host Control register.
 // 12. Set Byte_Swap_Non_Frame_Data and Byte_Swap_Data in the General Mode Control register (see “Mode
 // Control Register (offset: 0x6800)” on page 521).
 // 13. Set Word_Swap_Data and Word_Swap_Non_Frame_Data (optional). When the host processor
 // architecture is little-endian, set these additional bits in the General Mode Control register (see “Mode
 // Control Register (offset: 0x6800)” on page 521).
 // 14. Configure the Port Mode bits of the Ethernet MAC Mode register (see “EMAC Mode Register (offset:
 // 0x400)” on page 321) to GMII for all devices supporting copper Ethernet Media. Wait for 40 ms.
 // 15. Poll for bootcode completion. The device driver should poll the general communication memory at 0xB50
 // for the one's complement of the T3_MAGIC_NUMBER (i.e. 0xB49A89AB). The bootcode should complete
 // initialization within 1000 ms for flash devices and 10000 ms for SEEPROM devices.
 // 16. Enable/disable any required bug fixes. Refer to the applicable errata document for information on any
 // errata that should be worked around by enabling/disabling the control bits of chip bug fixes if any are
 // applicable.
 // 17. Enable Tagged Status Mode (optional) by setting the Enable_Tagged_Status_Mode bit of the Miscellaneous
 // Host Control register (see “Miscellaneous Host Control Register (offset: 0x68) — Function 0” on page 256).
 // For additional information on Tagged Status mode see “Interrupt Processing” on page 219.
 // 18. Clear the driver status block memory region. Write zeros to the host memory region where the status block
 // will be DMAed (see “Status Block” on page 82).
 // 19. Configure the DMA Write Water Mark in the DMA Read/Write Control register (see “DMA Read/Write
 // Control Register (offset: 0x6C) — Function 0” on page 258). If the Max Payload Size of PCIe Device Control
 // register is 128 bytes, set the DMA write water mark bits (bits19-21) of DMA Read/Write Control register to
 // 011b (for a water mark of 128 bytes). Otherwise (max payload size is 256 bytes or more), set the DMA write
 // water mark bits (bits 19–21) of DMA Read/Write Control register to 111b (for a watermark of 256 bytes).
 // 20. Set DMA byte swapping (optional). If the host processor architecture is big-endian, the MAC may byte swap
 // both control and frame data when acting as a PCI DMA master. Set the Byte_Swap_Non-Frame_Data,
 // Byte_Swap_Data and Word_Swap_Data bits in the General Mode Control register (see “Mode Control
 // Register (offset: 0x6800)” on page 521).
 // 21. Configure the host-based send ring. Set the Host_Send_BDs bit in the General Mode Control register (see
 // “Mode Control Register (offset: 0x6800)” on page 521).
 // 22. Indicate Driver is ready to receive traffic. Set the Host_Stack_Up bit in the General Mode Control register
 // (see “Mode Control Register (offset: 0x6800)” on page 521).
 // Note: For the BCM5725/BCM5762/BCM57767-PG106, do not touch DMA read/write control reg
 // (0x6C), and let the boot code have full control of it.
 // 23. Configure TCP/UDP pseudo header checksum offloading. This step is relevant when TCP/UDP checksum
 // calculations are offloaded to the device. The device driver may optionally disable receive and transmit
 // pseudo header checksum calculations by the device by setting the Receive_No_PseudoHeader_Checksum
 // and Send_No_PseudoHeader_Checksum bits in the General Mode Control register (see “Mode Control
 // Register (offset: 0x6800)” on page 521). If the Send_No_PsuedoHeader_Checksum bit is set, the host
 // software should make sure of seeding the correct pseudo header checksum value in TCP/UDP checksum
 // field. Similarly, if the Receive_No_PsuedoHeader_Checksum bit is set, the device driver should calculate
 // the pseudo header checksum and add it to the TCP/UDP checksum field of the received packet.
 // 24. Configure MAC Mbuf memory pool watermarks (“DMA MBUF Low Watermark Register (offset: 0x4414)”
 // on page 455, and “MBUF High Watermark Register (offset: 0x4418)” on page 455). Broadcom has run
 // hardware simulations on the Mbuf usage and strongly recommends the settings shown in the table below.
 // These settings/values establish proper operation for 10/100/1000 speeds.
 // 25. Configure flow control behavior when the RX Mbuf low watermark level has been reached (see table below
 // for “Low Watermark Maximum Receive Frame Register (offset: 0x504)” on page 337.
 // 26. Enable the buffer manager. The buffer manager handles the internal allocation of memory resources for
 // send and receive traffic. The Enable and Attn_Enable bits should be set in the Buffer Manager Mode
 // register (see “Buffer Manager Mode Register (offset: 0x4400)” on page 453.
 // 27. Set the BD ring replenish threshold for the RX producer ring. The threshold values indicate the number of
 // buffer descriptors that must be indicated by the host software before a DMA is initiated to fetch additional
 // receive descriptors used to replenish used receive descriptors. The recommended configuration value for
 // the Standard Receive BD Ring Replenish Threshold (see “Standard Receive BD Producer Ring Replenish
 // Threshold Register (offset: 0x2C18)” on page 382) is 0x19.
 // Table 55: Recommended BCM57XX Ethernet Controller Memory Pool Watermark Settings
 // Register Standard Ethernet Frames
 // MAC RX Mbuf Low Watermark (0x4414) 0x2A
 // Mbuf High Watermark (0x4418) 0xA0
 // Note: The Low WaterMark Max Receive Frames register (0x504) specifies the number of good frames
 // to receive after RxMbuf Low Watermark has been reached. The driver software should make sure
 // that the MAC RxMbuf Low WaterMark is greater than the number of Mbufs required for receiving the
 // number of frames as specified in 0x504. The first Mbuf in the Mbuf chain of a frame has 80 bytes of
 // packet data while each of the subsequent Mbufs [except the last Mbuf] has 120 bytes for packet data.
 // The last Mbuf in the chain has the rest of the packet data, which can be up to 120 bytes.
 // Table 56: Recommended BCM57XX Ethernet Controller Low Watermark Maximum Receive Frames Settings
 // Register Recommended Value
 // Low Water Mark Maximum Receive Frames (0x504) 1 
 // 28. Initialize the standard receive buffer ring. Host software should write the ring control block structure (see
 // “Ring Control Blocks” on page 124) to the Standard Receive BD Ring RCB register (see “Standard Receive BD
 // Ring RCB Registers” on page 379). Host software should be careful to initialize the host physical memory
 // address based on allocation routines specific to the OS.
 // 29. Initialize the Max_len/Flags Receive Ring RCB register (0x2458). Note that beginning with BCM5725/
 // BCM5762/BCM57767 and BCM5718 families of controllers, field 15:2 no specifies the maximum expected
 // size of a receive frame (it was formerly “Reserved”).
 // 30. Initialize the Receive Producer Ring NIC Address register (offset: 0x245C) to a value of 0x6000 for the
 // BCM5725/BCM5762/BCM57767.
 // 31. Initialize Receive BD Standard Producer Ring Index (0x26C).
 // 32. Initialize the Standard Ring Replenish Watermark register (offset: 0x2d00). The recommended value is
 // 0x20. If supporting Jumbo frames, then also initialize 0x2d04 (recommended value is 0x10). See section
 // “Jumbo Frames” on page 119 for more detail regarding supporting Jumbo frames.
 // 33. Initialize the Send Ring Producer Index registers. Clear (i.e., set to zero) the send BD ring host producer
 // index (see “Send BD Ring Host Producer Index (High Priority Mailbox) Register (offset: 0x300-0x307)” on
 // page 320).
 // 34. Initialize the send rings. The Send Ring Control Blocks (RCBs) are located in the miscellaneous memory
 // region from 0x100 to 0x1FF. Host software should be careful to initialize the host physical memory address
 // based on allocation routines specific to the OS. The MAC caches ¼ of the available send BDs in NIC local
 // memory, so the host driver must set up the NIC local address. The following formula should be used to
 // calculate the NIC Send Ring address: NIC Ring Address = 0x4000 + (Ring_Number *
 // sizeof(Send_Buffer_Descriptor) * NO_BDS_IN_RING) / 4
 // 35. Disable unused receive return rings. Host software should write the RCB_FLAG_RING_DISABLED bit to the
 // flags field of the ring control blocks of all unused receive return rings.
 // 36. Initialize receive return rings. The receive return ring RCBs are located in the miscellaneous memory region
 // from 0x200 to 0x2FF. Host software should be careful to initialize the host physical memory address based
 // on allocation routines specific to the OS. The Max_Len field indicates the ring size and it can be configured
 // to either 32 or 64 or 128 or 256 or 512. The NIC RingAddress field of the RCB has a hardware power-on
 // default value that is invalid for receive return rings, and the driver should set NIC Ring Address to 0x6000.
 // 37. Initialize the Receive Producer Ring mailbox registers. The driver should write the value 0x00000000 (clear)
 // to the low 32 bits of the receive BD standard producer ring index mailbox (see “Receive BD Standard
 // Producer Ring Index (High Priority Mailbox) Register (offset: 0x268-0x26f)” on page 319).
 // Note: Host software must insure that on systems that support more than 4 GB of physical memory,
 // send rings, receive return rings, producer rings, and packet buffers are not allocated across the 4 GB
 // memory boundary. For example, if the starting memory address of the standard receive buffer ring is
 // below 4 GB and the ending address is above 4 GB, a read DMA PCI host address overflow error may
 // be generated (see “Read DMA Status Register (offset: 0x4804)” on page 471.
 // Note: The standard RX producer threshold value should be set very low. Some OSs may run short of
 // memory resources and the number of BDs that are made available decrease proportionally.
 // 38. Configure the MAC unicast address. See “MAC Address Setup/Configuration” on page 144 for a full
 // description of unicast MAC address initialization.
 // 39. Configure random backoff seed for transmit. See the Ethernet Transmit Random Backoff register (see
 // “Ethernet Transmit Random Backoff Register (offset: 0x438)” on page 327). Broadcom recommends using
 // the following algorithm: Seed = (MAC_ADDR[0] + MAC_ADDR[1] + MAC_ADDR[2] + MAC_ADDR[3] + MAC_ADDR[4]
 // + MAC_ADDR[5]) & 0x3FF
 // 40. Configure the message transfer unit (MTU) size. The MTU sets the upper boundary on RX packet size;
 // packets larger than the MTU are marked oversized and discarded by the RX MAC. The MTU bit field in the
 // Receive MTU Size register (see “Receive MTU Size Register (offset: 0x43C)” on page 327) must be
 // configured before RX traffic is accepted. Host software should account for the following variables when
 // calculating the MTU:
 // • VLAN TAG
 // • CRC
 // • Jumbo frames enabled
 // 41. Configure the Inter-Packet Gap (IPG) for transmit. The Transmit MAC Lengths register (see “Transmit MAC
 // Lengths Register (offset: 0x464) ” on page 332) contains three bit fields: IPG_CRS_Length, IPG_Length, and
 // Slot_Time_Length. The value the 0x2620 should be written into this register.
 // 42. Configure default RX return ring for nonmatched packets. The MAC has a rules checker, and packets do not
 // always have a positive match. For this situation, host software must specify a default ring where RX packet
 // should be placed. The bit field is located in the Receive Rules Configuration register (see “Receive Rules
 // Configuration Register (offset: 0x500)” on page 336).
 // 43. Configure the number of receive Lists. The Receive List Placement Configuration register (see “Receive List
 // Placement Configuration Register (offset: 0x2010)” on page 375) allows host software to initialize QOS
 // rules checking. For example, a value of 0x181 (as used by Broadcom drivers) breaks down as follows:
 // • One interrupt distribution list
 // • Sixteen active lists
 // • One bad frames class
 // 44. Write the Receive List Placement Statistics mask. Broadcom drivers write a value of 0x7BFFFF (24 bits) to
 // the Receive List Placement Stats Enable Mask register (see “Receive List Placement Statistics Enable Mask
 // Register (offset: 0x2018)” on page 376).
 // 45. Enable RX statistics. Assert the Statistics_Enable bit in the Receive List Placement Control register (see
 // “Receive List Placement Statistics Control Register (offset: 0x2014)” on page 375).
 // 46. Enable the Send Data Initiator mask. Write 0xFFFFFF (24 bits) to the Send Data Initiator Enable Mask
 // register (see “Send Data Initiator Statistics Mask Register (offset: 0xC0C)” on page 358).
 // Note: The maximum number of send BDs for a single packet is (0.75)*(ring size).
 // Note: An incorrectly configured IPG introduces far-end receive errors on the MAC's link partner. 
 // 47. Enable TX statistics. Assert the Statistics_Enable and Faster_Statistics_Update bits in the Send Data Initiator
 // Control register (see “Send Data Initiator Statistics Control Register (offset: 0xC08)” on page 357.
 // 48. Disable the host coalescing engine. Software needs to disable the host coalescing engine before configuring
 // its parameters. Write 0x0000 to the Host Coalescing Mode register (see “Host Coalescing Mode Register
 // (offset: 0x3C00)” on page 443).
 // 49. Poll 20 ms for the host coalescing engine to stop. Read the Host Coalescing Mode register (see “Host
 // Coalescing Mode Register (offset: 0x3C00)” on page 443) and poll for 0x0000. The engine was stopped in
 // the previous step.
 // 50. Configure the host coalescing tick count. The Receive Coalescing Ticks and Send Coalescing Ticks registers
 // (see “Receive Coalescing Ticks Register (offset: 0x3C08)” on page 444 and “Send Coalescing Ticks Register
 // (offset: 0x3C0C)” on page 445) specify the number of clock ticks elapsed before an interrupt is driven. The
 // clock begins ticking after RX/TX activity. Broadcom recommends the settings shown in the table below.
 // 51. Configure the host coalescing BD count. The Receive Max Coalesced BD and Send Max Coalesced BD
 // registers (see “Receive Max Coalesced BD Count Register (offset: 0x3C10)” on page 446 and “Send Max
 // Coalesced BD Count Register (offset: 0x3C14)” on page 446 specify the number of frames processed before
 // an interrupt is driven. Broadcom recommends the settings shown in the table below.
 // 52. Configure the max-coalesced frames during interrupt counter. While host software processes interrupts,
 // this value is used. See the Receive Max Coalesced Frames During Interrupt and Send Max Coalesced Frames
 // During Interrupt registers (see “Receive Max Coalesced BD Count Register (offset: 0x3C10)” on page 446
 // and “Send Max Coalesced BD Count Register (offset: 0x3C14)” on page 446. Broadcom recommends the
 // settings shown in the table below.
 // Table 57: Recommended BCM57XX Ethernet Controller Host Coalescing Tick Counter Settings
 // Register Recommended Value
 // Receive Coalescing Ticks (0x3C08) 0x48
 // Send Coalescing Ticks (0x3C0C) 0x14
 // Table 58: Recommended BCM57XX Ethernet Controller Host Coalescing Frame Counter Settings
 // Register Recommended Value
 // Receive Max Coalesced Frames (0x3C10) 0x05
 // Send Max Coalesced Frames (0x3C14) 0x35
 // Table 59: Recommended BCM57XX Ethernet Controller Max Coalesced Frames During Interrupt
 // Counter Settings
 // Register Recommended Value
 // Receive Max Coalesced Frames During Interrupt (0x3C20) 0x05
 // Send Max Coalesced Frames During Interrupt (0x3C24) 0x05
 // 53. Initialize host status block address. Host software must write a physical address to the Status Block Host
 // Address register (see “Status Block Host Address Register (offset: 0x3C38)” on page 447, which is the
 // location where the MAC must DMA status data. This register accepts a 64-bit value in register 0x3C38 (high
 // order 32 bits) and 0x3C3C (low order 32 bits).
 // 54. Enable the host coalescing engine (0x3C00 bit 1).
 // 55. Enable the receive BD completion functional block. Set the Enable and Attn_Enable bits in the Receive BD
 // Completion Mode register (see “Receive BD Completion Mode Register (offset: 0x3000)” on page 384).
 // 56. Enable the receive list placement functional block. Set the Enable bit in the Receive List Placement Mode
 // register (see “Receive List Placement Mode Register (offset: 0x2000)” on page 374).
 // 57. Enable DMA engines. Set the Enable_FHDE, Enable_RDE, and Enable_TDE bits in the Ethernet Mac Mode
 // register (see “EMAC Mode Register (offset: 0x400)” on page 321.
 // 58. Enable and clear statistics. Set the Clear_TX_Statistics, Enable_TX_Statistics, Clear_RX_Statistics, and
 // Enable_TX_Statistics bits in the Ethernet Mac Mode register (see “EMAC Mode Register (offset: 0x400)” on
 // page 321).
 // 59. Delay 40 microseconds.
 // 60. Configure the General Miscellaneous Local Control register (see “Miscellaneous Local Control Register
 // (offset: 0x6808)” on page 524). Set the Interrupt_On_Attention bit for MAC to assert an interrupt whenever
 // any of the attention bits in the CPU event register are asserted.
 // 61. Delay 100 microseconds.
 // 62. Configure the Write DMA Mode register (see “Write DMA Mode Register (offset: 0x4C00)” on page 503).
 // The following bits are to be asserted:
 // • Enable-starts the functional block
 // • Write_DMA_PCI_Target_Abort_Attention_Enable
 // • Write_DMA_PCI_Master_Abort_Attention_Enable
 // • Write_DMA_PCI_Parity_Attention_Enable
 // • Write_DMA_PCI_Host_Address_Overflow_Attention_Enable
 // • Write_DMA_PCI_FIFO_Overerrun_Attention_Enable
 // • Write_DMA_PCI_FIFO_Underrun_Attention_Enable
 // • Write_DMA_PCI_FIFO_Overwrite_Attention_Enable
 // • Write_DMA_Local_Memory_Read_Longer_Than_DMA_Length
 // 63. Set bit-29 of the Write DMA Mode register (see “Write DMA Mode Register (offset: 0x4C00)” on page 503)
 // to enable the host coalescence block fix that configures the device to send out status block update before
 // the interrupt message.
 // 64. Delay 40 microseconds.
 // 65. Configure the Read DMA Mode register (see “Read DMA Programmable IPv6 Extension Header Register
 // (offset: 0x4808)” on page 472). The following bits are asserted:
 // • Enable-start functional block
 // Note: For the BCM5725/BCM5762/BCM57767-PG106, there is no need to touch this bit; keep the
 // default value for this bit.
 // • Read_DMA_PCI_Target_Abort
 // • Read_DMA_PCI_Master_Abort
 // • Read_DMA_PCI_Parity_Error
 // • Read_DMA_PCI_Host_Overflow_Error
 // • Read_DMA_PCI_FIFO_Overrun_Error
 // • Read_DMA_PCI_FIFO_Underrun_Error
 // • Read_DMA_PCI_FIFO_Overread_Error
 // • Read_DMA_Local_Memory_Write_Longer_Than_DMA_Length
 // 66. Delay 40 microseconds.
 // 67. Enable the receive data completion functional block. Set the Enable and Attn_Enable bits in the Receive
 // Data Completion Mode register (see “Receive Data Completion Mode Register (offset: 0x2800)” on
 // page 381).
 // 68. Enable the send data completion functional block. Set the Enable bit in the Send Data Completion Mode
 // register (see “Send Data Completion Mode Register (offset: 0x1000)” on page 365).
 // 69. Enable the send BD completion functional block. Set the Enable and Attn_Enable bits in the Send BD
 // Completion Mode register (see “Send BD Completion Mode Register (offset: 0x1C00)” on page 372).
 // 70. Enable the Receive BD Initiator Functional Block. Set the Enable and
 // Receive_BDs_Available_On_Receive_BD_Ring in the Receive BD Initiator Mode register (see “Receive BD
 // Initiator Mode Register (offset: 0x2C00)” on page 381).
 // 71. Enable the receive data and BD initiator functional block. Set the Enable and Illegal_Return_Ring_Size bits
 // in the Receive Data and Receive BD Initiator Mode register (see “Receive Data and Receive BD Initiator
 // Mode Register (offset: 0x2400)” on page 378).
 // 72. Enable the send data initiator functional block. Set the Enable bit in the Send Data Initiator Mode register
 // (see “Send Data Initiator Mode Register (offset: 0xC00)” on page 357).
 // 73. Enable the send BD initiator functional block. Set the Enable and Attn_Enable bits in the Send BD Initiator
 // Mode register (see “Send BD Initiator Mode Register (offset: 0x1800)” on page 370).
 // 74. Enable the send BD selector functional block. Set the Enable and Attn_Enable bits in the Send BD Selector
 // Mode register (see “Send BD Ring Selector Mode Register (offset: 0x1400)” on page 367).
 // 75. Replenish the receive BD producer ring with the receive BDs.
 // 76. Enable the transmit MAC. Set the Enable bit and the Enable_Bad_TxMBUF_Lockup_fix bit in the Transmit
 // MAC Mode register (see “Transmit MAC Mode Register (offset: 0x45C)” on page 330). Optionally, software
 // may set the Enable_Flow_Control to enable 802.3x flow control.
 // 77. Delay 100 microseconds.
 // 78. Enable the receive MAC. Set the Enable bit in the Receive MAC Mode register (see “Receive MAC Mode
 // Register (offset: 0x468)” on page 332). Optionally, software may set the following bits:
 // • Enable_Flow_Control-enable 802.3x flow control
 // • Accept_oversized-ignore RX MTU up to 64K maximum size
 // • Promiscuous_Mode-accept all packets regardless of destination address
 // • No_CRC_Check-RX MAC does not check Ethernet CRC
 // • Various Hash enable bits if using RSS mode (receive-side scaling) 
 // 79. Delay 10 microseconds.
 // 80. Set up the LED Control register (0x40C). The Broadcom driver uses a value of 0x800 when initializing this
 // register.
 // 81. Activate link and enable MAC functional blocks. Set the Link_Status bit in the MII Status register (see “MII
 // Status Register (offset: 0x450)” on page 328) to generate a link attention.
 // 82. Disable auto-polling on the management interface (optional) by writing 0xC0000 to the MI Mode register
 // (see “MII Mode Register (offset: 0x454) ” on page 329).
 // 83. Set Low Watermark Maximum Receive Frame register (offset: 0x504) to a value of 1 for the BCM5725/
 // BCM5762/BCM57767 controller.
 // 84. Configure D0 power state in PMSCR. See “Power Management Control/Status Register (offset: 0x4C) —
 //  Function 0” on page 253. Optional-the PMCSR register is reset to 0x00 after chip reset. Software may
 // optionally reconfigure this register if the device is being moved from D3 hot/cold.
 // 85. Set up the physical layer and restart auto-negotiation. For details on PHY auto-negotiation, refer to the
 // applicable PHY data sheet.
 // 86. Set up multicast filters. Refer to “Packet Filtering” on page 144 for details on multicast filter setup. 

 log("\nBROADCOM NETXTREME");

 //initalize interrupts
 set_irq_handler(ethernet_cards[number_of_card].irq, (dword_t)network_irq);
 //PCI 0x68
 // ec_broadcom_netxtreme_set_bits(number_of_card, 0x6808, (1 << 3));
 // ec_broadcom_netxtreme_set_bits(number_of_card, 0x5000, (1 << 7));
 // ec_broadcom_netxtreme_clear_bits(number_of_card, 0x5008, (1 << 12));
 // ec_broadcom_netxtreme_set_bits(number_of_card, 0x5028, 0x1);
 // ec_broadcom_netxtreme_set_bits(number_of_card, 0x408, (1 << 12) | (1 << 22) | (1 << 23));
 // ec_broadcom_netxtreme_set_bits(number_of_card, 0x450, (1 << 0));
 // ec_broadcom_netxtreme_set_bits(number_of_card, 0x6800, (1 << 26));
 // ec_broadcom_netxtreme_write_phy(number_of_card, 0x1B, 0x0000);

 // Clear existing interrupt bits for link changes
ec_broadcom_netxtreme_clear_bits(number_of_card, 0x5008, (1 << 12));

// Enable link change interrupt
ec_broadcom_netxtreme_set_bits(number_of_card, 0x5000, (1 << 7));

// Enable status check for link state changes
ec_broadcom_netxtreme_set_bits(number_of_card, 0x408, (1 << 12));

// Enable global interrupt enabling
ec_broadcom_netxtreme_set_bits(number_of_card, 0x6800, (1 << 26));

// Optionally configure PHY if necessary
ec_broadcom_netxtreme_write_phy(number_of_card, 0x1B, 0x0000);

 //generate interrupt
 // ec_broadcom_netxtreme_set_bits(number_of_card, 0x6808, (1 << 2));

 //Interrupt Mailbox 0 register “Interrupt Mailbox 0 (High Priority Mailbox) Register
 // (offset: 0x200-207)” on page 319 for host standard
 // and “Interrupt Mailbox 0 Register (offset: 0x5800)” on
 // page 513 for indirect mode.
 // Receive Coalescing Ticks register “Receive Coalescing Ticks Register (offset: 0x3C08)” on
 // page 444.
 // Send Coalescing Ticks register “Send Coalescing Ticks Register (offset: 0x3C0C)” on
 // page 445.
 // Receive Max Coalesced BD Count register “Receive Max Coalesced BD Count Register (offset:
 // 0x3C10)” on page 446.
 // Send Max Coalesced BD Count register “Send Max Coalesced BD Count Register (offset:
 // 0x3C14)” on page 446. 

 //0x400 promiscuos + MII/GMII

 //read MAC address from ports
 // for(dword_t i=0; i<6; i++) {
 //  ethernet_cards[number_of_card].mac_address[i] = (ec_intel_e1000_read(number_of_card, 0x5400+i) & 0xFF);
 // }

 //log MAC
 log("\nMAC: "); log_hex_with_space(ec_broadcom_netxtreme_read(number_of_card, 0x410)); log_hex(ec_broadcom_netxtreme_read(number_of_card, 0x414));
}

byte_t ec_broadcom_netxtreme_get_cable_status(dword_t number_of_card) {
 if((ec_broadcom_netxtreme_read(number_of_card, 0x460) & (1 << 3))==(1 << 3)) {
  return STATUS_TRUE;
 }
 else {
  return STATUS_FALSE;
 }
}

byte_t ec_broadcom_netxtreme_send_packet(dword_t number_of_card, byte_t *packet_memory, dword_t packet_size) {
 return STATUS_ERROR;
}

void ec_broadcom_netxtreme_process_irq(dword_t number_of_card) {
 pstr("irq");
 ec_broadcom_netxtreme_set_bits(number_of_card, 0x6808, (1 << 1));
}