#include <mts0_io.h>
#include <math.h>

Timestep::Timestep(string mts0_directory,int nx_, int ny_, int nz_) {
	nx = nx_;
	ny = ny_;
	nz = nz_;
	load_atoms(mts0_directory);
	remove_water();
}

Timestep::~Timestep() {
	positions.clear();
	atom_ids.clear();
	atom_types.clear();
	h_matrix.clear();
}

vector<int> Timestep::get_number_of_atoms_of_each_type() {
	int num_atoms = atom_types.size();
	vector<int> number_of_atoms_of_each_type;
	number_of_atoms_of_each_type.resize(100,0);
	for(int atom_index=0; atom_index<num_atoms; atom_index++) {
		number_of_atoms_of_each_type[ atom_types[atom_index] ]++;
	}

	return number_of_atoms_of_each_type;
}

Mts0_io::Mts0_io(int nx_, int ny_, int nz_, int max_timestep_, string foldername_base_, bool preload_) {
	nx = nx_;
	ny = ny_;
	nz = nz_;
	preload = preload_;
	foldername_base = foldername_base_;
	max_timestep = max_timestep_;
	if(preload) load_timesteps();
}

void Timestep::read_data(ifstream *file, void *value) {
	int N;
	file->read (reinterpret_cast<char*>(&N), sizeof(int));
	file->read (reinterpret_cast<char*>(value), N);
	file->read (reinterpret_cast<char*>(&N), sizeof(int));
}

void Timestep::read_mts(char *filename, vector<int> &atom_types_local, vector<int> &atom_ids_local, vector<vector<float> > &positions_local) {
	ifstream *file = new ifstream();
	file->open(filename, ios::in | ios::binary);
	if (!*file) {
		cout << "Error in Mts0_io::read_mts(): Failed to open file " << filename << endl;
		exit(1);
	}
	int num_atoms_local;
	read_data(file, &num_atoms_local);
	
	double *phase_space = new double[6*num_atoms_local];
	double *tmp_atom_data = new double[num_atoms_local];

	atom_types_local.resize(num_atoms_local);
	atom_ids_local.resize(num_atoms_local);
	positions_local.resize(num_atoms_local);
	for(int i=0;i<num_atoms_local;i++) {
		positions_local[i].resize(3);
	}

	read_data(file, tmp_atom_data);
	read_data(file, phase_space);

	for(int i=0;i<num_atoms_local;i++) {
		atom_types_local[i] = int(tmp_atom_data[i]);
		// Handle roundoff errors from 2 -> 1.99999999 -> 1
		atom_ids_local[i] = (tmp_atom_data[i]-atom_types_local[i])*1e11 + 1e-5;
		
		positions_local [i][0] = float(phase_space[3*i+0]);
		positions_local [i][1] = float(phase_space[3*i+1]);
		positions_local [i][2] = float(phase_space[3*i+2]);
	}

	double *tmp_h_matrix = new double[18];
	read_data(file,tmp_h_matrix);
	int count = 0;
	for(int k=0;k<2;k++) {
		for(int j=0;j<3;j++) {
			for(int i=0;i<3;i++) {
				h_matrix[k][i][j] = float(tmp_h_matrix[count++]);
			}
		}
	}

	file->close();

	delete tmp_h_matrix;
	delete phase_space;
	delete tmp_atom_data;
	delete file;
}

void Timestep::remove_water() {
	for(int n=0;n<get_number_of_atoms(); n++) {
		if(atom_types[n] == H_TYPE || atom_types[n] == O_TYPE) atom_is_removed[n] = true;
	}

	rearrange_vectors_by_moving_atoms_from_end_to_locations_where_atoms_have_been_removed();
}

void Timestep::rearrange_vectors_by_moving_atoms_from_end_to_locations_where_atoms_have_been_removed() {
	int num_atoms_including_removed_atoms = get_number_of_atoms();

	int remaining_atoms = 0;

	for(int n=0; n<num_atoms_including_removed_atoms; n++) {
		if(!atom_is_removed[n]) {
			atom_types[remaining_atoms] = atom_types[n];
			atom_ids[remaining_atoms] = atom_ids[n];
			positions[remaining_atoms] = positions[n];
			atom_is_removed[remaining_atoms] = false;
			remaining_atoms++;
		}
	}

	atom_types.erase(atom_types.begin()+remaining_atoms, atom_types.end());
	atom_ids.erase  (atom_ids.begin()  +remaining_atoms, atom_ids.end());
	positions.erase (positions.begin() +remaining_atoms, positions.end());
	atom_is_removed.erase(atom_is_removed.begin()+remaining_atoms, atom_is_removed.end());
}

void Timestep::load_atoms(string mts0_directory) {
	positions.clear();
	atom_types.clear();
	atom_ids.clear();
	h_matrix.clear();

	h_matrix.resize(2);
	for(int i=0;i<2;i++) {
		h_matrix[i].resize(3);
		for(int j=0;j<3;j++) {
			h_matrix[i][j].resize(3);
		}
	}
	vector<vector<float> > positions_local;
	vector<int> atom_types_local;
	vector<int> atom_ids_local;

	int num_nodes = nx*ny*nz;
	int num_atoms_local;
	vector<int> node_vector_index(3,0);
	vector<float> node_origin(3,0);
	vector<float> node_offset(3,0);

	char *filename = new char[1000];

	node_offset[0] = 1.0/nx;
	node_offset[1] = 1.0/ny;
	node_offset[2] = 1.0/nz;

	for(int node_id=0; node_id<num_nodes; node_id++) {
		node_vector_index[0] = node_id/(ny*nz);   // Node id in x-direction
		node_vector_index[1] = (node_id/nz) % ny; // Node id in y-direction
		node_vector_index[2] = node_id % nz; 	  // Node id in z-direction

		node_origin[0] = node_offset[0]*node_vector_index[0]; // Displacement in x-direction
		node_origin[1] = node_offset[1]*node_vector_index[1]; // Displacement in y-direction
		node_origin[2] = node_offset[2]*node_vector_index[2]; // Displacement in z-direction

		sprintf(filename,"%s/mt%04d",mts0_directory.c_str(), node_id);
		read_mts(filename, atom_types_local, atom_ids_local, positions_local);
		int num_atoms_local = positions_local.size();

		for(int j=0;j<num_atoms_local;j++) {
			for(int k=0;k<3;k++) {
				positions_local[j][k] += node_origin[k];
				positions_local[j][k] *= h_matrix[0][k][k]*bohr; // Possibly broken, we don't know how h_matrix really works
			}
		}

		positions.insert ( positions.end() , positions_local.begin() , positions_local.end()  );
		atom_types.insert ( atom_types.end() , atom_types_local.begin() , atom_types_local.end()  );
		atom_ids.insert ( atom_ids.end() , atom_ids_local.begin() , atom_ids_local.end()  );
	}

	atom_is_removed.resize( get_number_of_atoms() ,false);

	delete filename;
	atom_types_local.clear();
	atom_ids_local.clear();
	positions_local.clear();

	node_vector_index.clear();
	node_origin.clear();
	node_offset.clear();
}

vector<vector<int> > Timestep::create_neighbor_list(const float &neighbor_list_radius) {
	/*
	Strategy: Divide system into boxes of size neighbor_list_radius/3. Assign all atoms into their boxes O(n).
		      Loop through all boxes. Every atom is the neighbor of every other atom in this and the 26 neighboring boxes.
	*/
	float neighbor_list_radius_squared = neighbor_list_radius*neighbor_list_radius;
	vector<vector<int> > neighbor_list;
	int num_atoms = get_number_of_atoms();

	neighbor_list.resize(num_atoms);
	float box_size[3];
	int box_id[3];
	int num_boxes_xyz[3];
	vector<float> length = get_lx_ly_lz();
	for(int i=0;i<3;i++) {
		box_size[i] = neighbor_list_radius;
		num_boxes_xyz[i] = length[i]/box_size[i];
		box_size[i] = length[i]/num_boxes_xyz[i];
	}

	int num_boxes = num_boxes_xyz[0]*num_boxes_xyz[1]*num_boxes_xyz[2];
	
	vector<vector<vector<vector<int> > > > boxes;

	boxes.resize(num_boxes_xyz[0]);
	for(int i=0;i<num_boxes_xyz[0];i++) {
		boxes[i].resize(num_boxes_xyz[1]);
		for(int j=0;j<num_boxes_xyz[1];j++) {
			boxes[i][j].resize(num_boxes_xyz[2]);
		}
	}

	// Assign atoms to boxes
	for(int atom_index=0; atom_index<num_atoms; atom_index++) {
		// +1e-5 makes sure that box_id does not reach num_boxes when positions[][i]==length[i]
		for(int i=0;i<3;i++) box_id[i] = int(positions[atom_index][i]/(length[i]+1e-5)*num_boxes_xyz[i]);
		vector<int> &box = boxes[ box_id[0] ][ box_id[1] ][ box_id[2] ];
		box.push_back(atom_index);
	}

	// Loop over all boxes
	for(int i=0;i<num_boxes_xyz[0];i++) {
		for(int j=0;j<num_boxes_xyz[1];j++) {
			for(int k=0;k<num_boxes_xyz[2];k++) {
				vector<int> &box = boxes[i][j][k];
				int num_atoms_in_this_box = box.size();

				for(int n=0;n<num_atoms_in_this_box;n++) {
					// This box
					for(int m=n+1;m<num_atoms_in_this_box;m++) {
						neighbor_list[ box[n] ].push_back(box[m]);
						neighbor_list[ box[m] ].push_back(box[n]);
					}

					// Neighboring boxes (periodic boundaries)
					for(int di=-1;di<=1;di++) {
						for(int dj=-1;dj<=1;dj++) {
							for(int dk=-1;dk<=1;dk++) {
								if(di==0 && dj==0 && dk==0) continue; // Skip self
								int neighbor_box_i = (i + di + num_boxes_xyz[0]) % num_boxes_xyz[0];
								int neighbor_box_j = (j + dj + num_boxes_xyz[1]) % num_boxes_xyz[1];
								int neighbor_box_k = (k + dk + num_boxes_xyz[2]) % num_boxes_xyz[2];
								vector<int> &neighbor_box = boxes[neighbor_box_i][neighbor_box_j][neighbor_box_k];
								int num_atoms_in_neighbor_box = neighbor_box.size();

								for(int m=0;m<num_atoms_in_neighbor_box;m++) {
									neighbor_list[ box[n] ].push_back(neighbor_box[m]);
								}
							}
						}
					}
				}
			}
		}
	}
	length.clear();

	return neighbor_list;
}

int Timestep::get_number_of_atoms() {
	return positions.size();
}

vector<float> Timestep::get_lx_ly_lz() {
	vector<float> system_size(3);
	for(int i=0;i<3;i++) {
		system_size[i] = h_matrix[0][i][i]*bohr;
	}

	return system_size;
}

void Mts0_io::load_timesteps() {
	current_timestep = 0;
	
	timesteps.reserve(max_timestep+1);

	for(int timestep=0;timestep<=max_timestep;timestep++) {
		sprintf(mts0_directory, "%s/%06d/mts0/",foldername_base.c_str(), timestep);
		Timestep *new_timestep = new Timestep(string(mts0_directory),nx, ny, nz);
		if(remove_water) new_timestep->remove_water();
		timesteps.push_back(new_timestep);
		cout << "Loaded timestep " << timestep << endl;
	}
	system_size = timesteps[0]->get_lx_ly_lz();
}

Timestep *Mts0_io::get_next_timestep() {
	if(++current_timestep>max_timestep) current_timestep = 0;

	if(preload) {
		return timesteps[current_timestep];
	} else {
		sprintf(mts0_directory, "%s/%06d/mts0/",foldername_base.c_str(), current_timestep);
		Timestep *timestep = new Timestep(string(mts0_directory),nx, ny, nz);
		if(remove_water) timestep->remove_water();
		system_size = timestep->get_lx_ly_lz();
		return timestep;
	}
}