// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//


// listmap(const listmap&);
// copy ctor
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t, mapped_t, less_t>::listmap(const listmap& orig) {
   for (auto iter = orig.begin(); iter != orig.end(); ++iter) {
      insert(*iter);
   }
   less = *orig->less;
}


// listmap(listmap&&)
// move ctor
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t, mapped_t, less_t>::listmap(listmap&& orig) {
   for (auto iter = orig.begin(); iter != orig.end(); ++iter) {
      insert(*iter);
   }
   less = orig.less;

   while (!orig.empty()) {
      orig.erase(orig.begin());
   }
}


//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));

   while (!empty()) {
      erase(begin());
   }
}



//
// Iterator Operations
//

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);

   node* nn = new node(nullptr, nullptr, pair);

   auto pos_iter = begin();
   for (; pos_iter != end(); ++pos_iter) {  // look
      if (!less(pos_iter->first, pair.first)) { // found it
         if (!less(pair.first, pos_iter->first)) {  // replace value
            pos_iter = erase(pos_iter);
         }

         break;
      }
   }

   nn->prev = pos_iter.where->prev;
   nn->next = pos_iter.where;
   pos_iter.where->prev->next = nn;
   pos_iter.where->prev = nn;

   return iterator(nn);
}


//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);

   for (iterator iter = iterator(anchor()->next); iter != end(); 
         ++iter) {
      if (!less(iter->first, that) && !less(that, iter->first)) {
         return iter;
      }
   }
   return end();
}


//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);

   position.where->prev->next = position.where->next;
   position.where->next->prev = position.where->prev;
   
   iterator er_target = position;
   position++;
   delete er_target.where;
   
   return position;
}
