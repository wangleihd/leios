// Boot loader.
// 
// Part of the boot sector, along with bootasm.S, which calls bootmain().
// bootasm.S has put the processor into protected 32-bit mode.
// bootmain() loads an ELF kernel image from the disk starting at
// sector 1 and then jumps to the kernel entry routine.

#include "types.h"
#include "x86.h"

#define CRTPORT 0x3d4
#define LPTPORT 0x378
#define BACKSPACE 0x100
static ushort *crt = (ushort*)0xb8000;  // CGA memory
static char* message="This is an xv6 bootloader";
static char* prompt = "$";

// Copy console output to parallel port, which you can tell
// .bochsrc to copy to the stdout:
//   parport1: enabled=1, file="/dev/stdout"
static void
lpt_putc(int c)
{
  int i;

  for(i = 0; !(inb(LPTPORT+1) & 0x80) && i < 12800; i++)
    ;
  if(c == BACKSPACE)
    c = '\b';
  outb(LPTPORT+0, c);
  outb(LPTPORT+2, 0x08|0x04|0x01);
  outb(LPTPORT+2, 0x08);
}

static void
cga_putc(int c)
{
  int pos;
  
  // Cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);

  if(c == '\n')
    pos += 80 - pos%80;
  else if(c == BACKSPACE){
    if(pos > 0)
      crt[--pos] = ' ' | 0x0700;
  } else
    crt[pos++] = (c&0xff) | 0x0700;  // black on white
  
  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
  crt[pos] = ' ' | 0x0700;
}

void
cons_putc(int c)
{
  lpt_putc(c);
  cga_putc(c);
}


void
bootmain(void)
{
  int i;
  for(i=0;i<25;i++)
     cons_putc(message[i]);

  for(;;)
  {
  }
      
    
}


