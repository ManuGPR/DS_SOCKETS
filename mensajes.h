struct Peticion {
	// op coding -> 0 = init, 1 = set_value, 2 = get_value, 3 = modify_value, 4 = delete_key, 5 = exists; 
	int op;
	char value1[256];
	int N_value2;
	double V_value2[32];
	int key;
	char q_clientname[16];
};

struct Respuesta {
	int res;
	char value1[256];
	int N_or_exists;
	double V_value2[32];
};
