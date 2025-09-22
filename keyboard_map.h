//keyboard with just . -, space, backspace and letters

unsigned char keyboard_map[128] =
{
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 9 */
  0, '0', '-', 0, 0,	/* Backspace */
  0,			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', 0, 0, '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 0,	/* 39 */
 0, 0,   0,		/* Left shift */
 0, 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', 0, '.', 0,   0,				/* Right shift */
  0,
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};
