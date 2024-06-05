#if BOARD == BOARD_A
print("BOARD_A 1");
#else if BOARD == BOARD_B
print("BOARD_B 1");
#endif //BOARD

#if BOARD == BOARD_A
print("BOARD_A 2");
#elif BOARD == BOARD_B
print("BOARD_B 2");
#endif //BOARD

#if BOARD == BOARD_A
print("BOARD_A 3");
#else
print("BOARD_B 3");
#endif //BOARD

#if BOARD == BOARD_A
print("Test in BOARD_A 1");
#if 1 
print("Test in BOARD_A 2");
#endif 
#else if BOARD == BOARD_B
print("Test in BOARD_B 1");
#if 1 
print("Test in BOARD_B 2");
#endif 
#endif //BOARD


#if BOARD == BOARD_A
print("Only BOARD_A");
#endif //BOARD

#if BOARD == BOARD_B
print("Only BOARD_B");
#endif //BOARD

