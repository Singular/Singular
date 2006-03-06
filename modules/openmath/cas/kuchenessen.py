# -*- coding: utf-8 -*-
#Copyright Michael Brickenstein
#you are free to use this script under the terms of the Gnu LGPL
from sets import Set
import sets
class kuchen:
    __greater_equal_zero_set=Set()
    __not_greater_equal_zero_set=Set()
    __z=0
    def __init__(self,gebiete):
        if type(gebiete)==type(list()):
            self.gebiete=gebiete[:]
        else:
            self.gebiete=[]
            for (r,n) in gebiete:
                self.gebiete.extend([r]*n)

    def simplify(self):
        def quad_simple(x):
            (l,r)=x
            if (l %2==1) and(r %2==1):
                return (1,1)
            if l==r:
                return (1,1)
            else:
                return x
        

        gebiete=[quad_simple(a) for a in self.gebiete]
        g2=gebiete[:]
        for i in range(len(g2)):
            if g2[i]==None:
                continue
            (r,l)=g2[i]
            if (l,r) in g2[i+1:]:
                g2[i+1+ g2[i+1:].index((l,r))]=None
                g2[i]=None
        while None in g2:
            g2.remove(None)
        return kuchen(g2)
    def __char(self):
        d={}
        for i in self.gebiete:
            try:
                d[i]=d[i]+1
            except KeyError:
                d[i]=1
        s=Set()
        for k in d.keys():
            s.add((k,d[k]))
        return s

    def take_column(self, rect_n, column):
        """works only for column in the first half"""
        (l,r)=self.gebiete[rect_n]
        if column==0:
            if(r==1):
                g=[]
            else:
                g=[(l,r-1)]
        else:
            g=[(l,column),(l,r-1-column)]
        return kuchen(self.gebiete[:rect_n]+g+self.gebiete[rect_n+1:])
    
    def take_row(self, rect_n, row):
        """works only for row in the first half"""
        (l,r)=self.gebiete[rect_n]
        if row==0:
            if(l==1):
                g=[]
            else:
                g=[(l-1,r)]
        else:
            g=[(row,r),(l-1-row,r)]
        return kuchen(self.gebiete[:rect_n]+g+self.gebiete[rect_n+1:])
    
    def right_options(self):
        s=Set()
        for i in range(len(self.gebiete)):
            (l,r)=self.gebiete[i]
            if r %2==0:
                m=r/2
            else:
                m=r/2+1
            for j in range(m):
                s.add(self.take_column(i,j).__char())
        l=list(s)
        return [kuchen(k) for k in l]
    def right_options_for_geq0(self):
        """Not al and not necessarily the best right options,
        but if there is a right option <= 0, then the return
        Set will also contain such an option"""
        s=Set()
        if len(self.gebiete)>0:
            class strategy_failed(Exception):
                pass
            try:
                fuzzy=0
                for (l,r) in self.gebiete:
                    if (r%2==0) and (l!=r): 
                        raise strategy_failed
                    if (l%2==1) or (l==r):
                        fuzzy=fuzzy+1
                if fuzzy%2==1:
                    for i in range(len(self.gebiete)):
                        (l,r)=self.gebiete[i]
                        if (l%2==1) and (r%2==1):
                            if r==1:
                                j=0
                            else:
                                j=1
                            return [self.take_column(i,j)]
                        else:
                            if (l==r):
                                #even case
                                return [self.take_column(i,j)]
                            
                else:
                    for i in range(len(self.gebiete)):
                        if (l%2==0) and (r%2==1):
                            if r==1:
                                j=0
                            else:
                                j=1
                            return [self.take_column(i,j)]
                #shouldn't fail here in simplified games
                raise strategy_failed
            except strategy_failed:
                pass
        #fuzzy should be one or zero in simplified games
        for i in range(len(self.gebiete)):
            (l,r)=self.gebiete[i]
            if r %2==0:
                m=r/2
            else:
                m=r/2+1
            for j in range(m):
                if(l%2==1) and (r%2==1):
                    if (r>1) and (j!=1):
                        continue
                s.add(self.take_column(i,j).__char())
        l=list(s)
        return [kuchen(k) for k in l]

        
        
        
    def right_options_move(self):
        """Not al and not necessarily the best right options,
        but if there is a right option <= 0, then the return
        Set will also contain such an option"""
        d={}
        if len(self.gebiete)>0:
            class strategy_failed(Exception):
                pass
            try:
                fuzzy=0
                for (l,r) in self.gebiete:
                    if (r%2==0) and (l!=r): 
                        raise strategy_failed
                    if (l%2==1) or (l==r):
                        fuzzy=fuzzy+1
                if fuzzy%2==1:
                    for i in range(len(self.gebiete)):
                        (l,r)=self.gebiete[i]
                        if (l%2==1) and (r%2==1):
                            if r==1:
                                j=0
                            else:
                                j=1
                            return [(self.take_column(i,j),(i,j))]
                        else:
                            if (l==r):
                                #even case
                                return [(self.take_column(i,j),(i,j))]
                else:
                    for i in range(len(self.gebiete)):
                        if (l%2==0) and (r%2==1):
                            if r==1:
                                j=0
                            else:
                                j=1
                            return [(self.take_column(i,j),(i,j))]
                #shouldn't fail here in simplified games
                raise strategy_failed
            except strategy_failed:
                pass

        for i in range(len(self.gebiete)):
            (l,r)=self.gebiete[i]
            if r %2==0:
                m=r/2
            else:
                m=r/2+1
            for j in range(m):
                if(l%2==1) and (r%2==1):
                    if (r>1) and (j!=1):
                        continue
                d[sets.ImmutableSet(self.take_column(i,j).__char())]=(i,j)
       
        return [(kuchen(k),d[k]) for k in d.keys()]

    
    def left_options(self):
        s=Set()
        for i in range(len(self.gebiete)):
            (l,r)=self.gebiete[i]
            if l %2==0:
                m=l/2
            else:
                m=l/2+1
            for j in range(m):
                s.add(self.take_row(i,j).__char())
        l=list(s)
        return [kuchen(k) for k in l]
   
    def greater_equal_zero(self):
        k=self.simplify()
        if k.__char() in kuchen.__greater_equal_zero_set:
            return True
        if k.__char() in kuchen.__not_greater_equal_zero_set:
            return False
       
        erg=k.__do_greater_equal_zero()
        if erg:
            kuchen.__greater_equal_zero_set.add(k.__char())
        else:
            kuchen.__not_greater_equal_zero_set.add(k.__char())
        return erg
    
    def __do_greater_equal_zero(self):
        class myError(Exception):
            pass
        if len(self.gebiete)==1:
            (l,r)=self.gebiete[0]
            if (l%2==r%2) or (l==r):
                return False
        try:
            for i in range(len(self.gebiete)):
                (l,r)=self.gebiete[i]
                if (l % 2 == 0) or (r % 2 == 1):
                    raise myError
            return True
        except myError:
            pass
        
        complex_positiv_index=-1;
        complexity=0
        for i in range(len(self.gebiete)):
            (l,r)=self.gebiete[i]
            if (l%2==1) and (r%2==0) and l*r>complexity:
                complexity=l*r
                complex_positiv_index=i
        if complex_positiv_index>=0:
            rest=self.gebiete[:complex_positiv_index] \
                  + self.gebiete[complex_positiv_index+1:]
            if kuchen(rest).greater_equal_zero():
                return True
        #small rects have often bigger values
        not_complex_positiv_index=-1
        complexity=1000000000 #big number
        for i in range(len(self.gebiete)):
            (l,r)=self.gebiete[i]
            if (l%2==1) and (r%2==0) and l*r<complexity:
                complexity=l*r
                not_complex_positiv_index=i
        if not_complex_positiv_index>=0:
            rest=self.gebiete[:not_complex_positiv_index] \
                  + self.gebiete[not_complex_positiv_index+1:]
            #try to pick some other from rest
            j=0
            can=[self.gebiete[not_complex_positiv_index]]
            while (len(rest)>1) and (j<len(rest)):
                if j==not_complex_positiv_index:
                    j=j+1
                    continue

                if (rest[j][0]%2==1) and (rest[j][1]%2==0):
                    j=j+1
                    continue
                to_test=can[:]
                to_test.append(rest[j])
                if kuchen(to_test).greater_equal_zero():
                    can=to_test
                    rest.remove(rest[j])
                    continue
                j=j+1
            if kuchen(rest).greater_equal_zero():
                return True
        if len(self.gebiete)>0:
            try:
                for i in range(len(self.gebiete)):
                    (l,r)=self.gebiete[i]
                    if (l % 2==1) or (r % 2 ==0):
                        raise myError
                return False
            except myError:
                  complex_negativ_index=-1;
                  complexity=0
                  for i in range(len(self.gebiete)):
                      (l,r)=self.gebiete[i]
                      if (l%2==0) and (r%2==1) and l*r>complexity:
                          complexity=l*r
                          complex_negativ_index=i
                  if complex_negativ_index>=0:
                      if kuchen(self.gebiete[:complex_negativ_index] \
                                                   + self.gebiete[complex_negativ_index+1:]).smaller_equal_zero():
                          return False
        o=self.right_options_for_geq0()
        for i in range(len(o)):
            if o[i].smaller_equal_zero():
                return False
        return True
    
    def smaller_equal_zero(self):
        k=self.negative()
        return k.greater_equal_zero()
    def negative(self):
        return kuchen([(m[1],m[0]) for m in self.gebiete])


try:
	from objects import *
	import CD.gametheory
	def decode_kuchen_OM(kOM):
		assert isinstance(kOM,OMApply)
		sizes=[a.getValue() for a in kOM.args]
		regions=[(sizes[i],sizes[i+1]) for i in xrange(0, len(sizes),2)]
		return kuchen(regions)
	def left_wins_func(context, kuchen):
		kuchen=decode_kuchen_OM(kuchen)
		erg=kuchen.greater_equal_zero()
		if erg:
			return OMint(1)
		else:
			return OMint(0)
	def install_kuchenessen_plugin():
		CD.gametheory.implementation.implement("left_wins", left_wins_func)
except:
	print "no openmath library available"


	