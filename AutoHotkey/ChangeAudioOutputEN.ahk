+^!A::
	Run, mmsys.cpl
	WinWait,Sound
	ControlSend,SysListView321,{Down 5}
	ControlClick,&Set Default
	ControlClick,OK
	return

+^!B::
	Run, mmsys.cpl
	WinWait,Sound
	ControlSend,SysListView321,{Down 7}
	ControlClick,&Set Default
	ControlClick,OK
	return