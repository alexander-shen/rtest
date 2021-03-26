
// Test function returns the total amount of increases in the input sequence of 32-bit integers,
// i.e. the total amount of pairs of neighbouring numbers, of which the first is smaller than
// the second.
// param[3] is interpreted as the amount of integers in the sequence
bool runs(long double * value, unsigned long * hash, PRG gen, int * param, bool debug);

// Test function returns the total amount of increases in the input sequence of 32-bit integers,
// i.e. the total amount of pairs of neighbouring numbers, of which the first is smaller than
// the second.
// param[3] is interpreted as the number of tested integers




bool runs(long double * value, unsigned long * hash, PRG gen, int * param, bool debug)
{
    assert(param[2] == 1);

    int num_total = param[3];
    if(num_total < 2) {
	value[0] = 0;
	return true;
    }

    int increases = 0;
    
    unsigned int prev_num, cur_num;
    if(!g_int32_lsb(&prev_num, gen))
	return false;

    for(int i = 1; i < num_total; ++i) {
	if(!g_int32_lsb(&cur_num, gen))
	    return false;
	increases += cur_num > prev_num;
	prev_num = cur_num;
    }

    value[0] = increases;
    return true;
}


// Test function returns the total amount of bit shifts in the input sequence.
// param[3] is interpreted as the number of tested bytes

bool binary_runs(long double * value, unsigned long * hash, PRG gen, int * param, bool debug)
{
    assert(param[2] == 1);

    int bitshifts[256];

    // Counting amount of shifts in each byte as a small optimization.

    for(byte c = 0; c <= 255; ++c) {
	byte shifts_mask = (c ^ (byte)(c << 1)) & 254;
	
	//bitshifts[c] = __builtin_popcount(shifts_mask);
	
	bitshifts[c] = 0;
	for(char i = 0; i < 8; ++i)
	    bitshifts[c] += (shifts_mask & (1 << i) != 0);
    }

    int bytes_total = param[3];
    if(bytes_total == 0) {
	value[0] = 0;
	return true;
    }

    byte prev, next;
    if(!g_byte(&prev, gen))
	return false;

    int shifts_total = bitshifts[prev];

    for(int i = 1; i < bytes_total; ++i) {
	if(!g_byte(&next, gen))
	    return false;
	shifts_total += bitshifts[next];
	shifts_total += (prev & 128) ^ (next & 1);
	prev = next;
    }

    value[0] = shifts_total;
    return true;
}
