@echo off
mkdir gradriguito
cd gradriguito
del *.* /Q
copy ..\..\builds\vs_2017\Release\gradriguito.exe
copy ..\..\runtime\beat.gto
copy ..\..\runtime\readme.txt
copy ..\..\runtime\examples\beat.wav
cd ..
