class cl {
	int i;
	double d;
};

struct st {
	int i;
	double d;
	cl c;
};

union un {
	st s;
	cl c;
};
