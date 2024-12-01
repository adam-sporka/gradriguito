@echo off
mkdir gradriguito
cd gradriguito
del *.* /Q
copy ..\..\builds\vs_2017\Release\gradriguito.exe
copy ..\..\runtime\tutorial.gto
copy ..\..\runtime\before-basics.gto
copy ..\..\runtime\readme.txt
cd ..
