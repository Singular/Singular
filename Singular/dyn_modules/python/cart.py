import itertools
def xcombine(*seqin):
    '''returns a generator which returns combinations of argument sequences
for example xcombine((1,2),(3,4)) returns a generator; calling the next()
method on the generator will return [1,3], [1,4], [2,3], [2,4] and
StopIteration exception.  This will not create the whole list of
combinations in memory at once.'''
    def rloop(seqin,comb):
        '''recursive looping function'''
        if seqin:                   # any more sequences to process?
            for item in seqin[0]:
                newcomb=comb+[item]     # add next item to current combination
                # call rloop w/ remaining seqs, newcomb
                for item in rloop(seqin[1:],newcomb):
                    yield item          # seqs and newcomb
        else:                           # processing last sequence
            yield comb                  # comb finished, add to list
    return rloop(seqin,[])

def cartn(sequence, n):
  tocombine=list(itertools.repeat(sequence,n))
  return list(map(tuple,xcombine(*tocombine)))

if __name__=='__main__':
  for i in cartn([1,2,3],4):
    print(i)