#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#define ERROR0 "Divide Zero Exception"
#define ERROR6 "Invalid Operand Code Exception"

void divideZeroException();
void invalidOpcodeException();
void loadMain(void);

#endif /* INTERRUPTS_H_ */





