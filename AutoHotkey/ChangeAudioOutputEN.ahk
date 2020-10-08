+^!A::
	Run, mmsys.cpl
	WinWait,Sound
	ControlSend,SysListView321,{Down 1}
	ControlClick,&Default
	ControlClick,OK
	Return

+^!B::
	Run, mmsys.cpl
	WinWait,Sound
	ControlSend,SysListView321,{Down 2}
	ControlClick,&Default
	ControlClick,OK
	Reload
	Return