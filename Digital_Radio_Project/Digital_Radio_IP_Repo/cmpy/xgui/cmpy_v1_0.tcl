# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "A_B_MULT_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "A_TDATA_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "A_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "B_TDATA_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "B_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "Y_TDATA_WIDTH" -parent ${Page_0}
  ipgui::add_param $IPINST -name "Y_WIDTH" -parent ${Page_0}


}

proc update_PARAM_VALUE.A_B_MULT_WIDTH { PARAM_VALUE.A_B_MULT_WIDTH } {
	# Procedure called to update A_B_MULT_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.A_B_MULT_WIDTH { PARAM_VALUE.A_B_MULT_WIDTH } {
	# Procedure called to validate A_B_MULT_WIDTH
	return true
}

proc update_PARAM_VALUE.A_TDATA_WIDTH { PARAM_VALUE.A_TDATA_WIDTH } {
	# Procedure called to update A_TDATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.A_TDATA_WIDTH { PARAM_VALUE.A_TDATA_WIDTH } {
	# Procedure called to validate A_TDATA_WIDTH
	return true
}

proc update_PARAM_VALUE.A_WIDTH { PARAM_VALUE.A_WIDTH } {
	# Procedure called to update A_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.A_WIDTH { PARAM_VALUE.A_WIDTH } {
	# Procedure called to validate A_WIDTH
	return true
}

proc update_PARAM_VALUE.B_TDATA_WIDTH { PARAM_VALUE.B_TDATA_WIDTH } {
	# Procedure called to update B_TDATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.B_TDATA_WIDTH { PARAM_VALUE.B_TDATA_WIDTH } {
	# Procedure called to validate B_TDATA_WIDTH
	return true
}

proc update_PARAM_VALUE.B_WIDTH { PARAM_VALUE.B_WIDTH } {
	# Procedure called to update B_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.B_WIDTH { PARAM_VALUE.B_WIDTH } {
	# Procedure called to validate B_WIDTH
	return true
}

proc update_PARAM_VALUE.Y_TDATA_WIDTH { PARAM_VALUE.Y_TDATA_WIDTH } {
	# Procedure called to update Y_TDATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.Y_TDATA_WIDTH { PARAM_VALUE.Y_TDATA_WIDTH } {
	# Procedure called to validate Y_TDATA_WIDTH
	return true
}

proc update_PARAM_VALUE.Y_WIDTH { PARAM_VALUE.Y_WIDTH } {
	# Procedure called to update Y_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.Y_WIDTH { PARAM_VALUE.Y_WIDTH } {
	# Procedure called to validate Y_WIDTH
	return true
}


proc update_MODELPARAM_VALUE.A_WIDTH { MODELPARAM_VALUE.A_WIDTH PARAM_VALUE.A_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.A_WIDTH}] ${MODELPARAM_VALUE.A_WIDTH}
}

proc update_MODELPARAM_VALUE.B_WIDTH { MODELPARAM_VALUE.B_WIDTH PARAM_VALUE.B_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.B_WIDTH}] ${MODELPARAM_VALUE.B_WIDTH}
}

proc update_MODELPARAM_VALUE.Y_WIDTH { MODELPARAM_VALUE.Y_WIDTH PARAM_VALUE.Y_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.Y_WIDTH}] ${MODELPARAM_VALUE.Y_WIDTH}
}

proc update_MODELPARAM_VALUE.A_B_MULT_WIDTH { MODELPARAM_VALUE.A_B_MULT_WIDTH PARAM_VALUE.A_B_MULT_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.A_B_MULT_WIDTH}] ${MODELPARAM_VALUE.A_B_MULT_WIDTH}
}

proc update_MODELPARAM_VALUE.A_TDATA_WIDTH { MODELPARAM_VALUE.A_TDATA_WIDTH PARAM_VALUE.A_TDATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.A_TDATA_WIDTH}] ${MODELPARAM_VALUE.A_TDATA_WIDTH}
}

proc update_MODELPARAM_VALUE.B_TDATA_WIDTH { MODELPARAM_VALUE.B_TDATA_WIDTH PARAM_VALUE.B_TDATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.B_TDATA_WIDTH}] ${MODELPARAM_VALUE.B_TDATA_WIDTH}
}

proc update_MODELPARAM_VALUE.Y_TDATA_WIDTH { MODELPARAM_VALUE.Y_TDATA_WIDTH PARAM_VALUE.Y_TDATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.Y_TDATA_WIDTH}] ${MODELPARAM_VALUE.Y_TDATA_WIDTH}
}

