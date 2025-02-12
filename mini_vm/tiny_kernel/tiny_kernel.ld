OUTPUT_ARCH(aarch64)
ENTRY(start)

SECTIONS
{
	. = 0x80000;
	.text : {*(.text)}
}
