/* 
 * This library extends the performance of miller puckette's pure data (pd).
 * 
 * Copyright (C) 2000-2006 Thomas MUSIL [musil_at_iem.at]
 * Copyright (C) 2011 Alexandre Quessy <alexandre@quessy.net>
 * 
 * IEM - Institute of Electronic Music and Acoustics, Graz
 * Inffeldgasse 10/3, 8010 Graz, Austria
 * http://iem.at
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details ( GnuLGPL.txt ).
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** \file
 * [attributes]
 * Lists attributes of an abstraction.
 * 
 * Attributes are intial arguments given to an abstraction whose
 * name start with an "@" character, and whose value are the list of
 * atoms (symbols or floats) after each of them.
 */

#include "m_pd.h"
#include "g_canvas.h"

#define IS_A_FLOAT(atom,index) ((atom+index)->a_type == A_FLOAT)
#define IS_A_SYMBOL(atom,index) ((atom+index)->a_type == A_SYMBOL)

static t_class *attributes_class;

typedef struct _attributes
{
    t_object x_obj;
    t_outlet *x_outlet_ac;
    t_atom *x_at;
    int x_ac;
} t_attributes;

static void attributes_float(t_attributes *x, t_floatarg f)
{
    int i = (int) f;
    if ( ! i)
    {
        outlet_float(x->x_outlet_ac, x->x_ac);
        if (x->x_ac)
            outlet_list(x->x_obj.ob_outlet, &s_list, x->x_ac, x->x_at);
    }
    else if (i > 0)
    {
        if(i <= x->x_ac)
        {
            outlet_float(x->x_outlet_ac, i);
            if (IS_A_FLOAT(x->x_at, i-1))
                outlet_float(x->x_obj.ob_outlet, atom_getfloatarg(i-1, x->x_ac, x->x_at));
            else if (IS_A_SYMBOL(x->x_at, i-1))
                outlet_symbol(x->x_obj.ob_outlet, atom_getsymbolarg(i-1, x->x_ac, x->x_at));
        }
        else
            outlet_float(x->x_outlet_ac, 0);
    }
    else
    {
        int j = x->x_ac + i;
        
        if(j >= 0)
        {
            outlet_float(x->x_outlet_ac, j+1);
            if (IS_A_FLOAT(x->x_at, j))
                outlet_float(x->x_obj.ob_outlet, atom_getfloatarg(j, x->x_ac, x->x_at));
            else if (IS_A_SYMBOL(x->x_at, j))
                outlet_symbol(x->x_obj.ob_outlet, atom_getsymbolarg(j, x->x_ac, x->x_at));
        }
        else
            outlet_float(x->x_outlet_ac, 0);
    }
}

static void attributes_bang(t_attributes *x)
{
    attributes_float(x, 0.0f);
}

static void attributes_free(t_attributes *x)
{
    if (x->x_ac)
        freebytes(x->x_at, x->x_ac * sizeof(t_atom));
}

static void *attributes_new(void)
{
    post("New [attributes]");
    t_attributes *x = (t_attributes *) pd_new(attributes_class);
    t_glist *glist = (t_glist *) canvas_getcurrent();
    t_canvas *canvas = glist_getcanvas(glist);
    int pargc;
    t_atom *pargv;
    t_atom  *at;
    
    canvas_setcurrent(canvas);
    canvas_getargs(&pargc, &pargv);
    canvas_unsetcurrent(canvas);
    x->x_at = (t_atom *) getbytes(pargc * sizeof(t_atom));
    x->x_ac = pargc;
    at = x->x_at;
    while (pargc--)
        *at++ = *pargv++;
    outlet_new(&x->x_obj, &s_list);
    x->x_outlet_ac = outlet_new(&x->x_obj, &s_float);
    return x;
}

void attributes_setup(void)
{
    attributes_class = class_new(gensym("attributes"), (t_newmethod)attributes_new, (t_method)attributes_free, sizeof(t_attributes), CLASS_DEFAULT, A_GIMME, 0);
    class_addbang(attributes_class, (t_method) attributes_bang);
    class_addfloat(attributes_class, (t_method) attributes_float);
    post("[attributes] Parses attributes as arguments to an abstractions");
    post("Copyright (C) 2000-2006 Thomas MUSIL [musil_at_iem.at]");
    post("Copyright (C) 2011 Alexandre Quessy <alexandre@quessy.net>");
}

