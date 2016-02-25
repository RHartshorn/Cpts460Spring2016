echo killing pervious attempts ......
rm vdisk2
rm a.out

echo copying vdisk to vdisk2 .......
cp vdisk vdisk2

echo compliling.......
as86 -o bs.o bs.s
bcc -c -ansi bc.c

echo linking......
#remove io.o after bc.o to use kcs code
ld86 -d bs.o bc.o io.o /usr/lib/bcc/libc.a

echo check a.outs size
ls -l a.out

echo dump a.out to virtual hard drive
dd if=a.out of=vdisk2 bs=16 count=27 conv=notrunc
dd if=a.out of=vdisk2 bs=512 seek=1 conv=notrunc

echo launching qemu .....
qemu-system-i386 -hda vdisk2
 
