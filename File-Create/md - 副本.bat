@echo off
echo ��ӭʹ�������ļ��н����ű�QQ953082389
echo �������ʾ���롣���԰�ctrl+C�˳���
 
set /p val=��������Ҫ�������ļ�������
set /a varshu=%val%-1
for /l  %%i in (0,1,%val%-1) do(
md %%i
set /a file =%%i
for /l %%j in (0,1,100) do
(
md G:\pbs-zwhuang\File-Create\%%i\%%j
)
��
echo �ļ��д������
pause