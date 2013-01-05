//----------------------------  renumber_block_wise_01a.cc  ---------------------------
//    $Id$
//    Version: $Name$
//
//    Copyright (C) 2000, 2001, 2003, 2004, 2009, 2012 by the deal.II authors
//
//    This file is subject to QPL and may not be  distributed
//    without copyright and license information. Please refer
//    to the file deal.II/doc/license.html for the  text  and
//    further information on this license.
//
//----------------------------  renumber_block_wise_01a.cc  ---------------------------


// A redux of the _01 test that happened to fail on a branch to remove
// a bunch of iterator functions.



#include "../tests.h"
#include <deal.II/base/logstream.h>
#include <deal.II/base/function_lib.h>
#include <deal.II/lac/vector.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/hp/dof_handler.h>
#include <deal.II/dofs/dof_renumbering.h>
#include <deal.II/multigrid/mg_dof_handler.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_dgq.h>
#include <deal.II/fe/fe_dgp.h>
#include <deal.II/fe/fe_system.h>
#include <deal.II/hp/fe_collection.h>

#include <fstream>



template <int dim>
std::vector<unsigned int>
get_dofs (const hp::DoFHandler<dim> &dof)
{
  std::vector<unsigned int> local;
  std::vector<unsigned int> global;
  for (typename hp::DoFHandler<dim>::active_cell_iterator cell=dof.begin_active();
       cell != dof.end(); ++cell)
    {
      local.resize (cell->get_fe().dofs_per_cell);
      cell->get_dof_indices (local);

      global.insert (global.end(), local.begin(), local.end());
    }

  return global;
}



template <int dim>
void
check_renumbering(hp::DoFHandler<dim>& dof)
{
				   // do component-wise and save the
				   // results
  DoFRenumbering::component_wise (dof);
  const std::vector<unsigned int> vc = get_dofs (dof);

  deallog << "OK" << std::endl;
}


template <int dim>
void
check ()
{
  Triangulation<dim> tr;
  GridGenerator::hyper_cube(tr, -1,1);
  tr.refine_global (1);

  hp::DoFHandler<dim> dof(tr);
  {
    bool coin = false;
    for (typename hp::DoFHandler<dim>::active_cell_iterator cell=dof.begin_active();
	 cell != dof.end(); ++cell)
      {
	cell->set_active_fe_index (coin ? 0 : 1);
	coin = !coin;
      }
  }

  FESystem<dim> e1 (FE_Q<dim>(2), 1, FE_DGQ<dim>(0), 1);
  FESystem<dim> e2 (FE_Q<dim>(1), 1, FE_DGQ<dim>(0), 1);

  hp::FECollection<dim> fe_collection;
  fe_collection.push_back (e1);
  fe_collection.push_back (e2);

  dof.distribute_dofs(fe_collection);
  check_renumbering(dof);
  dof.clear();
}


int main ()
{
  std::ofstream logfile ("renumber_block_wise_01a/output");
  deallog << std::setprecision (2);
  deallog << std::fixed;
  deallog.attach(logfile);
  deallog.depth_console (0);

  deallog.push ("3d");
  check<3> ();
  deallog.pop ();
}
