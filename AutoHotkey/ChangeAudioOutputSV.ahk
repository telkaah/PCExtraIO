+^!A::
	Run, mmsys.cpl
	WinWait,Ljud
	ControlSend,SysListView321,{Down 1}
	ControlClick,&Standard
	ControlClick,OK
	Return

+^!B::
	Run, mmsys.cpl
	WinWait,Ljud
	ControlSend,SysListView321,{Down 4}
	ControlClick,&Standard
	ControlClick,OK
	Reload
	Return