/*
 * Hypermutationglobalerrorrate.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: quentin
 */

#include "Hypermutationglobalerrorrate.h"

using namespace std;

Hypermutation_global_errorrate::Hypermutation_global_errorrate(size_t nmer_width , Gene_class learn , Gene_class apply , double starting_flat_value){
	//Initialize booleans
	if(apply_to == V_gene | apply_to == VJ_genes | apply_to == VD_genes | apply_to == VDJ_genes){
		apply_to_v = true;
	}
	else apply_to_v = false;

	if(apply_to == D_gene | apply_to == DJ_genes | apply_to == VD_genes | apply_to == VDJ_genes){
		apply_to_d = true;
	}
	else apply_to_d = false;

	if(apply_to == J_gene | apply_to == VJ_genes | apply_to == DJ_genes | apply_to == VDJ_genes){
		apply_to_j = true;
	}
	else apply_to_j = false;

	if(learn_on == V_gene | learn_on == VJ_genes | learn_on == VD_genes | learn_on == VDJ_genes){
		learn_on_v = true;
	}
	else learn_on_v = false;

	if(learn_on == D_gene | learn_on == DJ_genes | learn_on == VD_genes | learn_on == VDJ_genes){
		learn_on_d = true;
	}
	else learn_on_d = false;

	if(learn_on == J_gene | learn_on == VJ_genes | learn_on == DJ_genes | learn_on == VDJ_genes){
		learn_on_j = true;
	}
	else learn_on_j = false;
}

Hypermutation_global_errorrate::~Hypermutation_global_errorrate() {
	// TODO Auto-generated destructor stub
	//Make a clean destructor and delete all the double* contained in maps
}

Error_rate* Hypermutation_global_errorrate::copy()const{

}

Error_rate* Hypermutation_global_errorrate::add_checked(Error_rate* err_r){

}

double Hypermutation_global_errorrate::get_err_rate_upper_bound() const{
	double max_proba = 0;
	for(i=0 ; i!=pow(4,mutation_Nmer_size);i++){
		if(Nmer_mutation_proba[i]>max_proba){
			max_proba = Nmer_mutation_proba[i];
		}
	}
	return max_proba;
}

double Hypermutation_global_errorrate::compare_sequences_error_prob (double scenario_probability , const string& original_sequence ,  Seq_type_str_p_map& constructed_sequences , const Seq_offsets_map& seq_offsets , const unordered_map<tuple<Event_type,Gene_class,Seq_side>, Rec_Event*>& events_map , Mismatch_vectors_map& mismatches_lists , double& seq_max_prob_scenario , double& proba_threshold_factor){
	//TODO Take into account the order of mutations

	string& v_gene_seq = (*constructed_sequences[V_gene_seq]);
	string& d_gene_seq = (*constructed_sequences[D_gene_seq]);
	string& j_gene_seq = (*constructed_sequences[J_gene_seq]);
	string& vd_ins_seq = (*constructed_sequences[VD_ins_seq]);
	string& vj_ins_seq = (*constructed_sequences[VJ_ins_seq]);
	string& dj_ins_seq = (*constructed_sequences[DJ_ins_seq]);

	scenario_resulting_sequence = v_gene_seq + vd_ins_seq + d_gene_seq + dj_ins_seq + vj_ins_seq + j_gene_seq; //Will this work?

	vector<int>& v_mismatch_list = *mismatches_lists[V_gene_seq];
	if(mismatches_lists.exist(D_gene_seq)){ //Remove check? ensured by initialization
		vector<int>& d_mismatch_list = *mismatches_lists[D_gene_seq];
	}

	vector<int>& j_mismatch_list = *mismatches_lists[J_gene_seq];

	scenario_new_proba = scenario_probability;

	//First compute the contribution of the errors to the sequence likelihood

	//Check that the sequence is at least the Nmer size
	tmp_len_util = scenario_resulting_sequence.size();
	if(tmp_len_util>=mutation_Nmer_size){
		current_mismatch = v_mismatch_list.begin();

		//TODO Need to get the previous V nucleotides and last J ones

		//Get the adress of the first Nmer
		Nmer_index = 0;
		for(i=0 ; i!=Nmer_mutation_proba ; i++){
			tmp_int_nt = stoi(v_gene_seq.substr(i,1));
			current_Nmer.push(tmp_int_nt);
			Nmer_index+=adressing_vector[i]*tmp_int_nt;
		}
		//FIXME maybe should iterate the other way around, what happens for errors/context of first nucleotides?

		//Check if there's an error and apply the cost accordingly
		if(current_mismatch==(mutation_Nmer_size-1)/2+1){
			scenario_new_proba*=Nmer_mutation_proba[Nmer_index];
			current_mismatch++;
		}
		else{
			scenario_new_proba*=(1-Nmer_mutation_proba[Nmer_index]);
		}

		//Look at all Nmers in the scenario_resulting_sequence by sliding window
		//Removing the contribution of the first
		for( i = (mutation_Nmer_size+1)/2 ; i!=scenario_resulting_sequence.size()-(mutation_Nmer_size-1)/2 ; i++){

			//Remove the previous first nucleotide of the Nmer and it's contribution to the index
			Nmer_index-=current_Nmer.front()*adressing_vector[mutation_Nmer_size-1];
			current_Nmer.pop();
			//Shift the index
			Nmer_index*=mutation_Nmer_size;
			//Add the contribution of the new nucleotide
			tmp_int_nt = stoi(scenario_resulting_sequence.substr(i+(mutation_Nmer_size-1)/2,1));//Assume a symetric Nmer
			Nmer_index+=tmp_int_nt;
			current_Nmer.push(tmp_int_nt);

			//Apply the error cost
			if(current_mismatch==00000){
				scenario_new_proba*=Nmer_mutation_proba[Nmer_index];
				current_mismatch++;
			}
			else{
				scenario_new_proba*=(1-Nmer_mutation_proba[Nmer_index]);
			}

		}

	}


/*	In order to be self consistent the error rate should be applied everywhere
 * //V gene
	if(apply_to_v){


	}


	//D gene
	if(apply_to_d){

	}


	//J gene
	if(apply_to_j){

	}*/

	//Record genomic nucleotides coverage and errors

	if(learn_on_v){

		//Get the coverage
		//Get the length of the gene and a pointer to the right array to write on
		tmp_corr_len = v_gene_nucleotide_coverage_seq_p[*vgene_real_index_p].first;
		tmp_cov_p = v_gene_nucleotide_coverage_seq_p[*vgene_real_index_p].second;
		tmp_err_p = v_gene_per_nucleotide_error_seq_p[*vgene_real_index_p].second;

		//Get the corrected number of deletions(no negative deletion)
		tmp_corr_len -= max(0,*v_3_del_value_p);

		// Compute the coverage
		for( i = max(0,-(*vgene_offset_p)) ; i != tmp_corr_len ; ++i ){
			tmp_cov_p[i]+=scenario_new_proba;
		}

		//Compute the error per nucleotide on the gene
		tmp_len_util = v_mismatch_list.size();
		for( i = 0 ; i != tmp_len_util ; ++i){
			tmp_err_p[v_mismatch_list[i]-(*vgene_offset_p)]+=scenario_new_proba;
		}

	}

	if(learn_on_d){

	}

	if(learn_on_j){

	}

}

queue<int> Hypermutation_global_errorrate::generate_errors(string& generated_seq , default_random_engine& generator) const{

}

void Hypermutation_global_errorrate::update(){

	//Update the error rate by maximizing the likelihood of the error model

	//Compute the mutation probabilities for the full Nmers


}

void Hypermutation_global_errorrate::initialize(const unordered_map<tuple<Event_type,Gene_class,Seq_side>, Rec_Event*>& events_map){
	//Get the right pointers for the V gene
	if(learn_on == V_gene | learn_on == VJ_genes | learn_on == VD_genes | learn_on == VDJ_genes){
		try{
			Gene_choice* v_gene_event_p = dynamic_cast<Gene_choice*>(events_map.at(tuple<Event_type,Gene_class,Seq_side>(GeneChoice_t,V_gene,Undefined_side)));
			vgene_offset_p = v_gene_event_p->alignment_offset_p;
			vgene_real_index_p = v_gene_event_p->current_realization_index;

			//Initialize gene counters
			const unordered_map<string , Event_realization>& v_realizations = v_gene_event_p->get_realizations_map();
			//Get the number of realizations
			n_v_real = v_realizations.size();
			//Create arrays
			v_gene_nucleotide_coverage_p = new pair<size_t,double*>[n_v_real];
			v_gene_per_nucleotide_error_p = new pair<size_t,double*>[n_v_real];
			v_gene_nucleotide_coverage_seq_p = new pair<size_t,double*>[n_v_real];
			v_gene_per_nucleotide_error_seq_p = new pair<size_t,double*>[n_v_real];

			for(unordered_map<string , Event_realization>::const_iterator iter = v_realizations.begin() ; iter != v_realizations.end() ; iter++){

				//Initialize normalized counters
				v_gene_nucleotide_coverage_p[(*iter).second.index] = pair<size_t,double*>((*iter).second.value_str_int.size(),new double [(*iter).second.value_str_int.size()]);
				v_gene_per_nucleotide_error_p[(*iter).second.index] = pair<size_t,double*>((*iter).second.value_str_int.size(),new double [(*iter).second.value_str_int.size()]);

				//Initialize sequence counters
				v_gene_nucleotide_coverage_seq_p[(*iter).second.index] = pair<size_t,double*>((*iter).second.value_str_int.size(),new double [(*iter).second.value_str_int.size()]);
				v_gene_per_nucleotide_error_seq_p[(*iter).second.index] = pair<size_t,double*>((*iter).second.value_str_int.size(),new double [(*iter).second.value_str_int.size()]);
			}

		}
		catch(exception& except){
			cout<<"Exception caught during initialization of Hypermutation global error rate"<<endl;
			cout<<"Exception caught trying to initialize V gene pointers"<<endl;
			cout<<endl<<"throwing exception now..."<<endl;
			throw except;
		}

		//Get deletion value pointer for V 3' deletions if it exists
		if(events_map.count(tuple<Event_type,Gene_class,Seq_side>(Deletion_t,V_gene,Three_prime)) != 0){
			const Deletion* v_3_del_event_p = dynamic_cast<Deletion*>(events_map.at(tuple<Event_type,Gene_class,Seq_side>(Deletion_t,V_gene,Three_prime)));
			v_3_del_value_p = &(v_3_del_event_p->deletion_value);
		}
		else{v_3_del_value_p = &no_del_buffer;}

	}

	//Get the right pointers for the D gene
	if(learn_on == D_gene | learn_on == DJ_genes | learn_on == VD_genes | learn_on == VDJ_genes){
		try{
			Gene_choice* d_gene_event_p = dynamic_cast<Gene_choice*>(events_map.at(tuple<Event_type,Gene_class,Seq_side>(GeneChoice_t,D_gene,Undefined_side)));
			dgene_offset_p = d_gene_event_p->alignment_offset_p;
			dgene_real_index_p = d_gene_event_p->current_realization_index;
			//Initialize gene counters

		}
		catch(exception& except){
			cout<<"Exception caught during initialization of Hypermutation global error rate"<<endl;
			cout<<"Exception caught trying to initialize D gene pointers"<<endl;
			cout<<endl<<"throwing exception now..."<<endl;
			throw except;
		}

		//Get deletion value pointer for D 5' deletions if it exists
		if(events_map.count(tuple<Event_type,Gene_class,Seq_side>(Deletion_t,D_gene,Five_prime)) != 0){
			const Deletion* d_5_del_event_p = dynamic_cast<Deletion*>(events_map.at(tuple<Event_type,Gene_class,Seq_side>(Deletion_t,D_gene,Five_prime)));
			d_5_del_value_p = &(d_5_del_event_p->deletion_value);
		}
		else{d_5_del_value_p = &no_del_buffer;}

		//Get deletion value pointer for D 3' deletions if it exists
		if(events_map.count(tuple<Event_type,Gene_class,Seq_side>(Deletion_t,D_gene,Three_prime)) != 0){
			const Deletion* d_3_del_event_p = dynamic_cast<Deletion*>(events_map.at(tuple<Event_type,Gene_class,Seq_side>(Deletion_t,D_gene,Three_prime)));
			d_3_del_value_p = &(d_3_del_event_p->deletion_value);
		}
		else{d_3_del_value_p = &no_del_buffer;}

	}

	//Get the right pointers for the J gene
	if(learn_on == J_gene | learn_on == DJ_genes | learn_on == VJ_genes | learn_on == VDJ_genes){
		try{
			Gene_choice* j_gene_event_p = dynamic_cast<Gene_choice*>(events_map.at(tuple<Event_type,Gene_class,Seq_side>(GeneChoice_t,J_gene,Undefined_side)));
			jgene_offset_p = j_gene_event_p->alignment_offset_p;
			jgene_real_index_p = j_gene_event_p->current_realization_index;
			//Initialize gene counters

		}
		catch(exception& except){
			cout<<"Exception caught during initialization of Hypermutation global error rate"<<endl;
			cout<<"Exception caught trying to initialize J gene pointers"<<endl;
			cout<<endl<<"throwing exception now..."<<endl;
			throw except;
		}
	}

	//Get deletion value pointer for J 5' deletions if it exists
	if(events_map.count(tuple<Event_type,Gene_class,Seq_side>(Deletion_t,J_gene,Five_prime)) != 0){
		const Deletion* j_5_del_event_p = dynamic_cast<Deletion*>(events_map.at(tuple<Event_type,Gene_class,Seq_side>(Deletion_t,J_gene,Five_prime)));
		j_5_del_value_p = &(j_5_del_event_p->deletion_value);
	}
	else{j_5_del_value_p = &no_del_buffer;}

}

void Hypermutation_global_errorrate::add_to_norm_counter(){
	if(seq_likelihood!=0){

		if(learn_on_v){
			for(i = 0 ; i!=n_v_real ; ++i){
				//Get the length of the gene and a pointer to the right array to write on
				tmp_corr_len = v_gene_nucleotide_coverage_seq_p[i].first;
				tmp_cov_p = v_gene_nucleotide_coverage_seq_p[i].second;
				tmp_err_p = v_gene_per_nucleotide_error_seq_p[i].second;

				double* tmp_cov_mod_p = v_gene_nucleotide_coverage_p[i].second;
				double* tmp_err_mod_p = v_gene_nucleotide_coverage_p[i].second;

				for(j = 0 ; j!= tmp_corr_len ; j++){
					//Add to normalize counter and reset coverage
					tmp_cov_mod_p[j] += tmp_cov_p[j]/seq_likelihood;
					tmp_cov_p[j] = 0;

					//Same for errors
					tmp_err_mod_p[j] += tmp_err_p[j]/seq_likelihood;
					tmp_err_p[j] = 0;
				}
			}
		}

		if(learn_on_d){

		}

		if(learn_on_j){

		}

		model_log_likelihood+=log10(seq_likelihood);
		number_seq+=1;
	}

	seq_mean_error_number = 0;
	seq_likelihood = 0;
	seq_probability = 0;
	debug_number_scenarios=0;
}

void Hypermutation_global_errorrate::clean_seq_counters(){
	if(seq_likelihood!=0){

			if(learn_on_v){
				for(i = 0 ; i!=n_v_real ; ++i){
					//Get the length of the gene and a pointer to the right array to write on
					tmp_corr_len = v_gene_nucleotide_coverage_seq_p[i].first;
					tmp_cov_p = v_gene_nucleotide_coverage_seq_p[i].second;
					tmp_err_p = v_gene_per_nucleotide_error_seq_p[i].second;

					for(j = 0 ; j!= tmp_corr_len ; j++){
						//reset coverage
						tmp_cov_p[j] = 0;

						//Same for errors
						tmp_err_p[j] = 0;
					}
				}
			}

			if(learn_on_d){

			}

			if(learn_on_j){

			}
	}


	seq_mean_error_number = 0;
	seq_likelihood = 0;
	seq_probability = 0;
	debug_number_scenarios=0;
}

void Hypermutation_global_errorrate::write2txt(ofstream& outfile){

}

void Hypermutation_global_errorrate::update_Nmers_proba(int current_pos , int current_index,double current_score){
	//Iterate through possible nucleotides at this position
	for(i=0;i!=4;i++){
		int new_index = current_index + i*adressing_vector[current_pos];
		double new_score = current_score*exp(ei_nucleotide_contributions[current_pos*4 +i]);
		if(current_pos!=mutation_Nmer_size-1){
			update_Nmers_proba(current_pos+1,new_index,new_score);
		}
		else{
			this->Nmer_mutation_proba[new_index]= new_score/Z;
		}
	}
}