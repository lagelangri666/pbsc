@echo off
echo 欢迎使用批量文件夹建立脚本QQ953082389
echo 请根据提示输入。可以按ctrl+C退出。
 
set /p val=请输入您要建立的文件夹数：
set /a varshu=%val%-1
for /l  %%i in (0,1,%val%-1) do md %%i
echo 文件夹创建完成
pause