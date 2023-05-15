Option Explicit
Dim s, objFSO, fso, oWS, sLinkFile, oLink,Path,Shortcut,f1
do
  WScript.StdOut.Write"1)Full name, group"& vbCrLf &"2)Deleting the contents of a given folder"& vbCrLf &"3)Creating a link to a given network resource and placing it on the desktop."& vbCrLf &"4)Exit"& vbCrLf
  Set WshShell=WScript.CreateObject("WScript.Shell")
  s = WScript.StdIn.ReadLine
  
  if(s="1") then 
    WScript.StdOut.Write"Gavrilchenko Aleksandr Evgenevich, ITP-11"
  
  elseif(s="2") then
Dim strSourceFolder
strSourceFolder = "C:\3"
Set objFSO = WScript.CreateObject("Scripting.FileSystemObject")
If objFSO.FolderExists(strSourceFolder) Then
	objFSO.DeleteFile objFSO.BuildPath(strSourceFolder, "*.*"), True
Else
	WScript.Echo "Can't find source folder [" & strSourceFolder & "]."
	WScript.Quit 1
End If
dim WshShell
Set objFSO = Nothing
WScript.Quit 0

  elseif(s="3") then
 Path = WshShell.SpecialFolders.item("Desktop")+"\"
		Set Shortcut = WshShell.CreateShortcut(Path & "testshrc.lnk")
		WScript.StdOut.Write "Enter the site path:"
 f1 = WScript.StdIn.ReadLine
		Shortcut.TargetPath = f1
		Shortcut.Save
  end if

loop until(s="4")