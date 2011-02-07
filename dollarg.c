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
 *   (e.g.  http://www.gnu.org/copyleft/lesser.html)
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * Graz, 22 November 2006
 *     Thomas Musil
 */

#include "m_pd.h"
#include "g_canvas.h"
#include "iemlib.h"

/* ------------------------- dollarg ---------------------------- */
/* --- dollar-arguments, output the initial-arguments and the --- */
/* ------- number of them of the parent abstraction-patch ------- */

static t_class *dollarg_class;

typedef struct _dollarg
{
  t_object  x_obj;
  void      *x_outlet_ac;
  t_atom    *x_at;
  int       x_ac;
} t_dollarg;

static void dollarg_float(t_dollarg *x, t_floatarg f)
{
  int i = (int)f;
  
  if(!i)
  {
    outlet_float(x->x_outlet_ac, x->x_ac);
    if(x->x_ac)
      outlet_list(x->x_obj.ob_outlet, &s_list, x->x_ac, x->x_at);
  }
  else if(i > 0)
  {
    if(i <= x->x_ac)
    {
      outlet_float(x->x_outlet_ac, i);
      if(IS_A_FLOAT(x->x_at, i-1))
        outlet_float(x->x_obj.ob_outlet, atom_getfloatarg(i-1, x->x_ac, x->x_at));
      else if(IS_A_SYMBOL(x->x_at, i-1))
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
      if(IS_A_FLOAT(x->x_at, j))
        outlet_float(x->x_obj.ob_outlet, atom_getfloatarg(j, x->x_ac, x->x_at));
      else if(IS_A_SYMBOL(x->x_at, j))
        outlet_symbol(x->x_obj.ob_outlet, atom_getsymbolarg(j, x->x_ac, x->x_at));
    }
    else
      outlet_float(x->x_outlet_ac, 0);
  }
}

static void dollarg_bang(t_dollarg *x)
{
  dollarg_float(x, 0.0f);
}

static void dollarg_free(t_dollarg *x)
{
  if(x->x_ac)
    freebytes(x->x_at, x->x_ac * sizeof(t_atom));
}

static void *dollarg_new(void)
{
  t_dollarg *x = (t_dollarg *)pd_new(dollarg_class);
  t_glist *glist=(t_glist *)canvas_getcurrent();
  t_canvas *canvas=glist_getcanvas(glist);
  int pargc;
  t_atom *pargv, *at;
  
  canvas_setcurrent(canvas);
  canvas_getargs(&pargc, &pargv);
  canvas_unsetcurrent(canvas);
  x->x_at = (t_atom *)getbytes(pargc*sizeof(t_atom));
  x->x_ac = pargc;
  at = x->x_at;
  while(pargc--)
    *at++ = *pargv++;
  outlet_new(&x->x_obj, &s_list);
  x->x_outlet_ac = outlet_new(&x->x_obj, &s_float);
  return (x);
}

void dollarg_setup(void)
{
  dollarg_class = class_new(gensym("dollarg"), (t_newmethod)dollarg_new,
    (t_method)dollarg_free, sizeof(t_dollarg), 0, 0);
  class_addcreator((t_newmethod)dollarg_new, gensym("$n"), 0);
  class_addbang(dollarg_class, (t_method)dollarg_bang);
  class_addfloat(dollarg_class, (t_method)dollarg_float);
//  class_sethelpsymbol(dollarg_class, gensym("iemhelp/help-dollarg"));
}
