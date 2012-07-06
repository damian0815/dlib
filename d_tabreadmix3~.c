/*
 *   tabreadmix.c  -  an overlap add tabread~ clone
 *   Copyright (c) 2000-2003 by Tom Schouten
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <m_pd.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/******************** tabreadmix~ ***********************/

static t_class *d_tabreadmix3_tilde_class;

// cos lut
static const float smoothing[257]= {
 0.000000f, 
 0.000151f,  0.000602f,  0.001355f,  0.002408f,  0.003760f, 
 0.005412f,  0.007361f,  0.009607f,  0.012149f,  0.014984f, 
 0.018112f,  0.021529f,  0.025235f,  0.029227f,  0.033503f, 
 0.038060f,  0.042895f,  0.048005f,  0.053387f,  0.059039f, 
 0.064956f,  0.071135f,  0.077572f,  0.084264f,  0.091207f, 
 0.098395f,  0.105826f,  0.113494f,  0.121395f,  0.129523f, 
 0.137875f,  0.146445f,  0.155229f,  0.164219f,  0.173412f, 
 0.182802f,  0.192383f,  0.202149f,  0.212094f,  0.222213f, 
 0.232500f,  0.242947f,  0.253549f,  0.264300f,  0.275193f, 
 0.286221f,  0.297378f,  0.308657f,  0.320051f,  0.331553f, 
 0.343157f,  0.354856f,  0.366642f,  0.378508f,  0.390448f, 
 0.402453f,  0.414517f,  0.426633f,  0.438793f,  0.450989f, 
 0.463216f,  0.475464f,  0.487727f,  0.499998f,  0.512269f, 
 0.524532f,  0.536780f,  0.549007f,  0.561203f,  0.573363f, 
 0.585479f,  0.597543f,  0.609549f,  0.621488f,  0.633354f, 
 0.645140f,  0.656839f,  0.668443f,  0.679946f,  0.691340f, 
 0.702619f,  0.713776f,  0.724804f,  0.735696f,  0.746447f, 
 0.757049f,  0.767497f,  0.777783f,  0.787902f,  0.797848f, 
 0.807614f,  0.817195f,  0.826585f,  0.835778f,  0.844769f, 
 0.853552f,  0.862122f,  0.870474f,  0.878603f,  0.886504f, 
 0.894172f,  0.901602f,  0.908791f,  0.915733f,  0.922425f, 
 0.928863f,  0.935042f,  0.940959f,  0.946611f,  0.951994f, 
 0.957104f,  0.961939f,  0.966496f,  0.970771f,  0.974763f, 
 0.978469f,  0.981887f,  0.985015f,  0.987851f,  0.990392f, 
 0.992638f,  0.994588f,  0.996239f,  0.997592f,  0.998645f, 
 0.999398f,  0.999849f,  1.000000f,  0.999849f,  0.999398f, 
 0.998645f,  0.997593f,  0.996240f,  0.994589f,  0.992639f, 
 0.990393f,  0.987852f,  0.985016f,  0.981889f,  0.978471f, 
 0.974765f,  0.970773f,  0.966497f,  0.961941f,  0.957106f, 
 0.951996f,  0.946613f,  0.940962f,  0.935045f,  0.928866f, 
 0.922428f,  0.915736f,  0.908794f,  0.901606f,  0.894175f, 
 0.886507f,  0.878606f,  0.870478f,  0.862126f,  0.853556f, 
 0.844772f,  0.835782f,  0.826589f,  0.817199f,  0.807618f, 
 0.797852f,  0.787907f,  0.777788f,  0.767501f,  0.757054f, 
 0.746452f,  0.735701f,  0.724808f,  0.713780f,  0.702624f, 
 0.691345f,  0.679950f,  0.668448f,  0.656844f,  0.645145f, 
 0.633359f,  0.621493f,  0.609554f,  0.597548f,  0.585484f, 
 0.573368f,  0.561209f,  0.549012f,  0.536786f,  0.524537f, 
 0.512274f,  0.500003f,  0.487733f,  0.475469f,  0.463221f, 
 0.450995f,  0.438798f,  0.426638f,  0.414522f,  0.402458f, 
 0.390453f,  0.378513f,  0.366647f,  0.354861f,  0.343162f, 
 0.331558f,  0.320056f,  0.308662f,  0.297383f,  0.286226f, 
 0.275197f,  0.264305f,  0.253554f,  0.242952f,  0.232504f, 
 0.222218f,  0.212099f,  0.202153f,  0.192387f,  0.182806f, 
 0.173416f,  0.164223f,  0.155232f,  0.146449f,  0.137879f, 
 0.129527f,  0.121398f,  0.113497f,  0.105829f,  0.098398f, 
 0.091210f,  0.084267f,  0.077575f,  0.071138f,  0.064958f, 
 0.059041f,  0.053390f,  0.048007f,  0.042897f,  0.038062f, 
 0.033505f,  0.029229f,  0.025237f,  0.021531f,  0.018113f, 
 0.014985f,  0.012150f,  0.009608f,  0.007362f,  0.005412f, 
 0.003761f,  0.002408f,  0.001355f,  0.000602f,  0.000151f,
 0.0f

};

typedef struct _d_tabreadmix3_tilde
{
    t_object x_obj;
    int npoints;
    float *x_vec;
    t_symbol *x_arrayname;
    float x_f;
	t_outlet* list_outlet;

	// requested blocksize
	float desired_blocksize;

    /* file position vars */
	float curr_window_start; // in buffer units
    float prev_window_start;
	float curr_window_pos_pct;	// 0..1 
	float prev_window_pos_pct;
	float window_width; // in buffer units
	//int window_steps; // int
    //int curr_window_steps_remaining; // store window steps remaining
	//int prev_window_steps_remaining;
	float window_inc; // in buffer units
    
    int wrap_end;

} t_d_tabreadmix3_tilde;



void d_tabreadmix3_tilde_audiorate(t_d_tabreadmix3_tilde *x, t_float f)
{
	// limit settings
    if ( f<0.001 ) 
		f = 0.001;
	if ( f > 0.49f*x->window_width )
		f = 0.49f*x->window_width;
	printf("audiorate %f -> \n", f );

	// store old params
	x->window_inc = f;
//	x->window_steps = x->window_width / x->window_inc;
	//x->prev_window_steps_remaining = (1.0f-prev_window_pos_pct) * x->window_steps;
	//x->curr_window_steps_remaining = (1.0f-curr_window_pos_pct) * x->window_steps;
	//printf(" inc %f window width %i\n", x->window_inc );

}



#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)>(y)?(x):(y))


static t_int *d_tabreadmix3_tilde_perform(t_int *w)
{
    t_d_tabreadmix3_tilde *x = (t_d_tabreadmix3_tilde *)(w[1]);
	// position input (eg from phasor~)
	// goes from 0..1
    t_float *pos = (t_float *)(w[2]);
	// output
    t_float *out = (t_float *)(w[3]);
	// number of samples
	int num_samples = (int)(w[4]);
	// source buffer
	float* buf = x->x_vec;
	int i;

    if (!buf || x->npoints <= 1024 || x->wrap_end <= 1024 ) 
	{
	    for ( i=0; i<num_samples; i++) 
			*out++ = 0;
		return (w+5);
	}
	
	// when pos = 0, prevpos window is min 0..2r
	// so currpos window is min r..3r
	// when pos = 1, currpos window is max wrap_end-2r..wrap_end
	// so prevpos window is max wrap_end-3r..wrap_end-r
	
	// --> given pos, if window is 2r wide then
	// prevpos window left is at
	// 0..pos*(wrap_end-3r)
	// and currpos window left is at
	// pos*r..pos*wrap_end-2r
	// (given window left, window right is window left+2r)
	/*static const int NUM_CHUNK_RECORD_SLOTS = 256*3;
	int chunksizes[NUM_CHUNK_RECORD_SLOTS];
	int num_chunks_recorded = 0;*/

	// perform
	//prinf("perform: num_samples is 
	int chunk_zerosize = 0;
	while (num_samples > 0)
	{
		// smoothing via sin lut
		// linear interpolation, start/end set by lut lookup
		int curr_window_steps_remaining = (1.0f-x->curr_window_pos_pct)*x->window_width/x->window_inc;
		int prev_window_steps_remaining = (1.0f-x->prev_window_pos_pct)*x->window_width/x->window_inc;

		// chunk size is the least number of steps we can make without crossing a window boundary
		int chunk = min(min(prev_window_steps_remaining,curr_window_steps_remaining), num_samples);
		float old_curr_window_pos_pct = x->curr_window_pos_pct;


		float curr_pos = x->curr_window_start + x->curr_window_pos_pct*x->window_width;
		float prev_pos = x->prev_window_start + x->prev_window_pos_pct*x->window_width;
		float smoothing_pos_curr = x->curr_window_pos_pct*256;
		float smoothing_pos_prev = x->prev_window_pos_pct*256;
		float smoothing_pos_step = 256.0f*x->window_inc/x->window_width;
	
		if ( chunk == 0 )
		{
			if ( chunk_zerosize )
			{
				/*
				printf( "curr: %5i-%5i %f-%f %f:%f prev: %5i-%5i %f-%f %f:%f\n", 
					(int)curr_pos, (int)(curr_pos+x->window_inc*chunk), 
					x->curr_window_pos_pct, x->curr_window_pos_pct+chunk*x->window_inc/x->window_width,
					smoothing[(int)smoothing_pos_curr], smoothing[(int)(smoothing_pos_curr+smoothing_pos_step*chunk)],
					(int)prev_pos, (int)(prev_pos+x->window_inc*chunk),
					x->prev_window_pos_pct, x->prev_window_pos_pct+chunk*x->window_inc/x->window_width,
					smoothing[(int)smoothing_pos_prev], smoothing[(int)(smoothing_pos_prev+smoothing_pos_step*chunk)] );
					*/
				break;
			}

			chunk_zerosize = 1;
		}	

		for ( i=0; i<chunk; i++ )
		{
			// calculate indices
			float curr_index_a = curr_pos;
			int curr_index_b = curr_index_a+1;
			float prev_index_a = prev_pos;
			int prev_index_b = prev_index_a+1;
			// interpolate
			float curr_interp_factor = curr_index_a-(int)curr_index_a;
			float prev_interp_factor = prev_index_a-(int)prev_index_a;
			float curr = curr_interp_factor*buf[curr_index_b] + (1.0f-curr_interp_factor)*buf[(int)curr_index_a];
			float prev = prev_interp_factor*buf[prev_index_b] + (1.0f-prev_interp_factor)*buf[(int)prev_index_a];

			/*
			// smoothing
			int smoothing_curr_index_a = smoothing_pos_curr;
			int smoothing_curr_index_b = smoothing_curr_index_a+1;
			int smoothing_prev_index_a = smoothing_pos_prev;
			int smoothing_prev_index_b = smoothing_prev_index_a+1;
			// interpolate
			curr_interp_factor = smoothing_curr_index_a-(int)smoothing_curr_index_a;
			prev_interp_factor = smoothing_prev_index_a-(int)smoothing_prev_index_a;
			float smoothing_curr = curr_interp_factor*smoothing[(int)smoothing_curr_index_b] + (1.0f-curr_interp_factor)*buf[(int)smoothing_curr_index_a];
			float smoothing_prev = prev_interp_factor*smoothing[(int)smoothing_prev_index_b] + (1.0f-prev_interp_factor)*buf[(int)smoothing_prev_index_a];*/
			
			// calculate + store 
			float smoothing_curr = smoothing[(int)smoothing_pos_curr];
			float smoothing_prev = smoothing[(int)smoothing_pos_prev];
			*out++ = smoothing_curr*curr + smoothing_prev*prev;

			/*smoothing_curr += smoothing_step_curr;
			smoothing_prev += smoothing_step_prev;*/
			
			// update positions in window
			smoothing_pos_curr += smoothing_pos_step;
			smoothing_pos_prev += smoothing_pos_step;
			curr_pos += x->window_inc;
			prev_pos += x->window_inc;

		}
		x->curr_window_pos_pct = (curr_pos - x->curr_window_start)/x->window_width;
		x->prev_window_pos_pct = (prev_pos - x->prev_window_start)/x->window_width;
		curr_window_steps_remaining = (1.0f-x->curr_window_pos_pct)*x->window_width/x->window_inc;
		prev_window_steps_remaining = (1.0f-x->prev_window_pos_pct)*x->window_width/x->window_inc;

		// log
		//printf("played chunk of %4i (%4i remaining): curr [%f %f] pos %f-%f, steps left %i %i\n", chunk, num_samples, x->curr_window_start, x->curr_window_start+x->window_width, old_curr_window_pos_pct, x->curr_window_pos_pct, curr_window_steps_remaining, prev_window_steps_remaining );
		
		// advance pos
		pos += chunk;
		num_samples -= chunk;
	
		// window finished?
		float potential_next_start_pos = *pos;
		if ( curr_window_steps_remaining <= 0 )
		{
			// next window for curr
			float p = potential_next_start_pos;
		//	printf("  curr window finished: potential next start pos %f\n", p );
			float r = 0.5f*x->window_width;
			x->curr_window_start = p*(x->wrap_end - 2*r);
			x->curr_window_pos_pct = 0.0f;
			// keep in sync
			x->prev_window_pos_pct = 0.5f;
		//	printf("      new window at [%f %f]\n", x->curr_window_start, x->curr_window_start+2*r );
		}
		if ( prev_window_steps_remaining <= 0 )
		{
			// next window for prev
			float p = potential_next_start_pos;
		//	printf("  prev window finished: potential next start pos %f\n", p );
			float r = 0.5f*x->window_width;
			x->prev_window_start = p*(x->wrap_end - 2*r);
			x->prev_window_pos_pct = 0.0f;
			// keep in sync
			x->curr_window_pos_pct = 0.5f;
		//	printf("      new window at [%f %f]\n", x->curr_window_start, x->curr_window_start+2*r);
		}


		
		
	}
	// tidy up the rest of the output
	for ( i=0; i<num_samples; i++) 
		*out++ = 0;
	/*
	static char result[4096+128];
	result[0] = 0;
	int rsize=0;
	i=0;
	while ( rsize < 4096 && i < num_chunks_recorded )
	{
		char bit[128];
		sprintf(bit, " %i:%i-%i", chunksizes[i], chunksizes[i+1], chunksizes[i+2] );
		strcat( result, bit );
		rsize += strlen( bit );
		i+=3;
	}
	post( result );
	*/

	return (w+5);
}



// block size is the window width
static void d_tabreadmix3_tilde_blocksize(t_d_tabreadmix3_tilde *x, t_float size)
{
	//printf(" - blocksize %f, wrap_end %i\n", size, x->wrap_end );
	//printf(" - blocksize: %f requested\n", size );
	x->desired_blocksize = size;
	// maintain current position
	float curr_pos = x->curr_window_start + x->curr_window_pos_pct*x->window_width;
	float prev_pos = x->prev_window_start + x->prev_window_pos_pct*x->window_width;
	
	// adjust window width
	if ( size > x->wrap_end )
		size = x->wrap_end;


	if ( size < max(2,2.1f*x->window_inc) )
		size = max(2,2.1f*x->window_inc);
	
	// set new window
	float curr_window_start = curr_pos - x->curr_window_pos_pct*size;
	float prev_window_start = prev_pos - x->prev_window_pos_pct*size;
	// keep within bounds
	if ( curr_window_start < 0 )
		curr_window_start = 0;
	if ( prev_window_start < 0 )
		prev_window_start = 0;
	if ( curr_window_start + size > x->wrap_end )
		curr_window_start = x->wrap_end-size;
	if ( prev_window_start + size > x->wrap_end )
		prev_window_start = x->wrap_end-size;

	x->window_width 	= size;
	x->curr_window_start = curr_window_start;
	x->prev_window_start = prev_window_start;

	// info
	//printf(" - blocksize: array is from %i to %i\n", 0, x->wrap_end );
	//printf(" - blocksize: final windows: [%f %f]:[%f %f] (width %f)\n", x->prev_window_start, x->prev_window_start+x->window_width, x->curr_window_start, x->curr_window_start+x->window_width, x->window_width );
	//printf(" - blocksize: final pos %%: %f %f\n", x->prev_window_pos_pct, x->curr_window_pos_pct);
}

void d_tabreadmix3_tilde_wrap(t_d_tabreadmix3_tilde *x, t_float start, t_float end )
{
    if ( start > end )
    	error( "d_tabreadmix3~: wrap: start %i must be lower than end %i", (int)start, (int)end );
    else if ( 0 > start || start > x->npoints )
    	error( "d_tabreadmix3~: wrap: start %i must be between 0 and bufsize (%d)", (int)start, (int)x->npoints );
    else if ( 0 > end || end > x->npoints )
    	error( "d_tabreadmix3~: wrap: end %i must be between 0 and bufsize (%d)", (int)end, (int)x->npoints );
    else if ( start > 0 )
		error("d_tabreadmix3~: wrap: start %i must be 0 (sorry)", (int)start );
	{
    	x->wrap_end = end;
		// recalculate windows
		d_tabreadmix3_tilde_blocksize(x, x->desired_blocksize );
    }
}

/*void d_tabreadmix3_tilde_pitch(t_d_tabreadmix3_tilde *x, t_float f)
{
    if (f < 1) f = 1;

    d_tabreadmix3_tilde_blocksize(x, sys_getsr() / f);
}*/



/*void d_tabreadmix3_tilde_chunks(t_d_tabreadmix3_tilde *x, t_float f)
{
    if (f < 1.0f) f = 1.0f;
    d_tabreadmix3_tilde_blocksize(x, (float)x->x_npoints / f);
}*/

/*void d_tabreadmix3_tilde_bang(t_d_tabreadmix3_tilde *x, t_float f)
{
    //trigger a chunk reset on next dsp call
    x->x_xfade_phase = x->x_xfade_size;
}*/

void d_tabreadmix3_tilde_set(t_d_tabreadmix3_tilde *x, t_symbol *s)
{
    t_garray *a;
    

    x->x_arrayname = s;
	//printf("-- using array %s in dsp\n", x->x_arrayname->s_name );
    if (!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
    {
        if (*s->s_name)
            error("tabreadmix~: %s: no such array", x->x_arrayname->s_name);
        x->x_vec = 0;
    }
    else if (!garray_getfloatarray(a, &x->npoints, &x->x_vec))
    {
        error("%s: bad template for tabreadmix~", x->x_arrayname->s_name);
        x->x_vec = 0;
        // reset wrappage
        x->wrap_end = x->npoints;
    }
    else 
	{
		garray_usedindsp(a);
	}
	
	// use blocksize method to reset bits
	d_tabreadmix3_tilde_blocksize( x, x->desired_blocksize );

	//post("d_tabreadmix3_tilde_set: npoints %i xvec %i", x->x_npoints, x->x_vec );
	//
    /* make sure indices are inside array */
    /*if (x->x_npoints == 0){
		x->x_currpos = 0;
		x->x_prevpos = 0;
    }*/

    //else d_tabreadmix3_tilde_wrapindices(x);

	//printf("-- array set\n");
}

static void d_tabreadmix3_tilde_dsp(t_d_tabreadmix3_tilde *x, t_signal **sp)
{
    d_tabreadmix3_tilde_set(x, x->x_arrayname);

    dsp_add(d_tabreadmix3_tilde_perform, 4, x,
        sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);

}

static void d_tabreadmix3_tilde_free(t_d_tabreadmix3_tilde *x)
{
}

static void *d_tabreadmix3_tilde_new(t_symbol *s)
{
	post("d_tabreadmix3~ loaded");
    t_d_tabreadmix3_tilde *x = (t_d_tabreadmix3_tilde *)pd_new(d_tabreadmix3_tilde_class);
    x->x_arrayname = s;
    x->x_vec = 0;
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("blocksize"));  
    outlet_new(&x->x_obj, gensym("signal"));
	x->list_outlet = outlet_new(&x->x_obj, gensym("list"));
    /* file position vars */
	x->curr_window_start = 0;// in buffer units
    x->prev_window_start = 512;
	x->curr_window_pos_pct = 0;	
	x->prev_window_pos_pct = 0.5;
	x->window_width = 1024; // in buffer units
	x->window_inc = 1; // in buffer units
    //x->curr_window_steps_remaining = 1024; // store window steps remaining
	//x->prev_window_steps_remaining = 512;
    
    x->wrap_end = 0;

	d_tabreadmix3_tilde_blocksize(x, 1024);

	
	return (x);
}

void d_tabreadmix3_tilde_setup(void)
{
    d_tabreadmix3_tilde_class = class_new(gensym("d_tabreadmix3~"),
        (t_newmethod)d_tabreadmix3_tilde_new, (t_method)d_tabreadmix3_tilde_free,
        sizeof(t_d_tabreadmix3_tilde), 0, A_DEFSYM, 0);
    CLASS_MAINSIGNALIN(d_tabreadmix3_tilde_class, t_d_tabreadmix3_tilde, x_f);
    class_addmethod(d_tabreadmix3_tilde_class, (t_method)d_tabreadmix3_tilde_dsp,
        gensym("dsp"), 0);
    class_addmethod(d_tabreadmix3_tilde_class, (t_method)d_tabreadmix3_tilde_set,
        gensym("set"), A_SYMBOL, 0);
    class_addmethod(d_tabreadmix3_tilde_class, (t_method)d_tabreadmix3_tilde_blocksize,
        gensym("blocksize"), A_FLOAT, 0);
    /*class_addmethod(d_tabreadmix3_tilde_class, (t_method)d_tabreadmix3_tilde_pitch,
        gensym("pitch"), A_FLOAT, 0);
    class_addmethod(d_tabreadmix3_tilde_class, (t_method)d_tabreadmix3_tilde_chunks,
        gensym("chunks"), A_FLOAT, 0);*/
    class_addmethod(d_tabreadmix3_tilde_class, (t_method)d_tabreadmix3_tilde_audiorate,
		gensym("audiorate"), A_FLOAT, 0 );
    class_addmethod(d_tabreadmix3_tilde_class, (t_method)d_tabreadmix3_tilde_wrap,
		gensym("wrap"), A_FLOAT, A_FLOAT, 0 );	
    /*class_addmethod(d_tabreadmix3_tilde_class, (t_method)d_tabreadmix3_tilde_bang,
        gensym("bang"), 0);*/
    
}

