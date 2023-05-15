Option Explicit
do
Option Explicit

Dim strSourceFolder
Dim objFSO


strSourceFolder = "C:\Мои проекты\0221\Source"

Set objFSO = WScript.CreateObject("Scripting.FileSystemObject")

If objFSO.FolderExists(strSourceFolder) Then
	objFSO.DeleteFile objFSO.BuildPath(strSourceFolder, "*.*"), True
Else
	WScript.Echo "Can't find source folder [" & strSourceFolder & "]."
	WScript.Quit 1
End If

Set objFSO = Nothing

WScript.Quit 0
loop until(objFSO="4")


 dim DesktopPath
DesktopPath = WSHShell.SpecialFolders("Desktop")
set MyShortcut = WSHShell.CreateShortcut(DesktopPath + "\forall.lnk")
MyShortcut.TargetPath = WSHShell.ExpandEnvironmentStrings("\\192.168.0.30\forall")
MyShortcut.Description = "urluk_for_forall"
MyShortcut.WindowStyle = 1
MyShortcut.IconLocation = WSHShell.ExpandEnvironmentStrings("%SystemRoot%\system32\SHELL32.dll, 19")
MyShortcut.Save()
