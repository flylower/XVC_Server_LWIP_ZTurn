#set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN P14} [get_ports tms]
#set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN W14} [get_ports tdo]
#set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN U15} [get_ports tdi]
#set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN Y14} [get_ports tck]
set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN P14 PULLUP true} [get_ports tms]
set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN U15 PULLUP true} [get_ports tck]
set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN W14} [get_ports tdo]
set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN Y14 PULLUP true} [get_ports tdi]

#set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN P14} [get_ports tms]
#set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN U15} [get_ports tck]
#set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN W14} [get_ports tdo]
#set_property -dict {IOSTANDARD LVCMOS33 PACKAGE_PIN Y14} [get_ports tdi]