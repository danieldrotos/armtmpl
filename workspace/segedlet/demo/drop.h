#ifndef DROP_H_
#define DROP_H_


enum drop_state_t {
	ds_none= 0,
	ds_run= 1,
	ds_paused= 2,
	ds_done= 3
};


extern void drop();


#endif /* EJTES_H_ */
