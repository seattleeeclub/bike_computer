Use Tiva_Libraries insted of the compiled tivaware.  Interrupt vector table in the precompiled version
does not work with Rowley.  I think there's a writup in one of the project here 
that describes the error and how to get around it.

Also, use the Tiva_Libraries in this top level and don't copy to each project.


Added 7/16/17:
From one of the projects:

NOTE:
Getting the TivaWare to work with Rowley Crossworks is not trivial.
Examples from Tivaware use the precompiled standard driver libs.  These 
Don't work well with Rowley because of the VTABLE .vectors, and how it 
assigns the memory block for VTABLE.  If you try to use interrupt, it
will generate an error, something about UNUSED SECTIONS overlapping 
into .init section.  

if you look at the Tiva user manual, it says the vtable needs to be 
mapped somewhere into ram and recommends mapping these to the top of 
ram, or 0x20000000.  The file interrupt.c contains the code that 
does this.  You need to modify this to place vtable into a section in 
ram.  I did this by assigning it  to .data.  I'm not sure if this is the 
best way to do it, but at least it works.  



Changes to interrupt.c:
The following is the change that one needs to make to get 
the tiva libs to work with rowley crossworks:

//*****************************************************************************
//
// The processor vector table.
//
// This contains a list of the handlers for the various interrupt sources in
// the system.  The layout of this list is defined by the hardware; assertion
// of an interrupt causes the processor to start executing directly at the
// address given in the corresponding location in this list.
//
//*****************************************************************************
//
// Set the size of the vector table to the largest number of interrupts of
// any device
//

//this is the original code - does not work 
/*
#undef NUM_INTERRUPTS
#define NUM_INTERRUPTS                          155
#if defined(ewarm)
#pragma data_alignment=1024
static __no_init void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void) @ "VTABLE";
#elif defined(sourcerygxx)
static __attribute__((section(".cs3.region-head.ram")))
void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void) __attribute__ ((aligned(1024)));
#elif defined(ccs) || defined(DOXYGEN)
#pragma DATA_ALIGN(g_pfnRAMVectors, 1024)
#pragma DATA_SECTION(g_pfnRAMVectors, ".vtable")
void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void);
#else
static __attribute__((section("vtable")))
void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void) __attribute__((aligned(1024)));
#endif

*/

/////////////////////////////////////////////////////////
//The "fix" for this was to put vtable in .data section.  
//The user manual says a good place to put this in top of the 
//ram section , or 0x20000000.  I can't figure out how to do that
//Probably in the flash placement file, define a section aligned on 1024
//call it "my table", then define a starting address for it.
//PUT IT IN DATA
//

#undef NUM_INTERRUPTS
#define NUM_INTERRUPTS                          155
#if defined(ewarm)
#pragma data_alignment=1024
static __no_init void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void) @ "VTABLE";
#elif defined(sourcerygxx)
static __attribute__((section(".cs3.region-head.ram")))
void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void) __attribute__ ((aligned(1024)));
#elif defined(ccs) || defined(DOXYGEN)
#pragma DATA_ALIGN(g_pfnRAMVectors, 1024)
#pragma DATA_SECTION(g_pfnRAMVectors, ".vtable")
void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void);
#else
static __attribute__((section(".data")))
void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void) __attribute__((aligned(1024)));
#endif



//PUT IT IN RODATA
//
/*
#undef NUM_INTERRUPTS
#define NUM_INTERRUPTS                          155
#if defined(ewarm)
#pragma data_alignment=1024
static __no_init void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void) @ "VTABLE";
#elif defined(sourcerygxx)
static __attribute__((section(".cs3.region-head.ram")))
void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void) __attribute__ ((aligned(1024)));
#elif defined(ccs) || defined(DOXYGEN)
#pragma DATA_ALIGN(g_pfnRAMVectors, 1024)
#pragma DATA_SECTION(g_pfnRAMVectors, ".vtable")
void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void);
#else
static __attribute__((section(".rodata")))
void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void) __attribute__((aligned(1024)));
#endif


*/



