 /*
 *   interrupts.h
 *
 *  Created on: Apr 18, 2010
 *      Author: anizzomc
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

void _irq00Handler(void);
void _irq01Handler(void);
void _irq02Handler(void);
void _irq03Handler(void);
void _irq04Handler(void);
void _irq05Handler(void);

void _exception0Handler(void);
void _exception6Handler(void);

void _cli(void);
void _sti(void);

void _hlt(void);

void picMasterMask(uint8_t mask);
void picSlaveMask(uint8_t mask);

void syscallINTHandler(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

/* Ends cpu execution */
void haltcpu(void);

uint64_t * getSavedRegisters(void);

#endif /* INTERRUPTS_H_ */
