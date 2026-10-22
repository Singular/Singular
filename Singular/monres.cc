/* Monomial minimal resolutions: Lyubeznik cancellation and support-lattice
 * homology. All implementation lives in this translation unit.
 * Differential entries are field coefficients plus implicit monomial labels;
 * finished black boxes use immutable compressed sparse columns.
 */
#include "kernel/mod2.h"
#include "coeffs/coeffs.h"
#include "misc/options.h"
#include <cstddef>
#include <map>
#include <vector>
#include <stdexcept>
namespace monres {
using Degree = std::vector<long>;
class Scalar {
  struct Adopt {};
  Scalar(number n, coeffs cf, Adopt): value(n), field(cf) {}
public:
  number value;
  coeffs field;
  Scalar(long n, coeffs cf): value(n_Init(n, cf)), field(cf) {}
  Scalar(const Scalar& n): value(n_Copy(n.value, n.field)), field(n.field) {}
  Scalar(Scalar&& n) noexcept: value(n.value), field(n.field) { n.field=NULL; }
  Scalar& operator=(const Scalar& n) {
    if(this!=&n) { number v=n_Copy(n.value,n.field); if(field) n_Delete(&value,field); value=v; field=n.field; } return *this;
  }
  Scalar& operator=(Scalar&& n) noexcept {
    if(this!=&n) { if(field) n_Delete(&value,field); value=n.value; field=n.field; n.field=NULL; } return *this;
  }
  ~Scalar() { if(field) n_Delete(&value,field); }
  bool isZero() const { return n_IsZero(value,field); }
  Scalar operator+(const Scalar& n) const { return Scalar(n_Add(value,n.value,field),field,Adopt()); }
  Scalar operator-(const Scalar& n) const { return Scalar(n_Sub(value,n.value,field),field,Adopt()); }
  Scalar operator*(const Scalar& n) const { return Scalar(n_Mult(value,n.value,field),field,Adopt()); }
  Scalar operator/(const Scalar& n) const {
    if(n.isZero()) throw std::runtime_error("zero pivot in monres");
    return Scalar(n_Div(value,n.value,field),field,Adopt());
  }
};
using Column=std::map<int,Scalar>;
struct Cell { Degree degree; Column boundary; };
using Complex=std::vector<std::vector<Cell>>;
Complex lyubeznik(const std::vector<Degree>&,coeffs,size_t);
Complex squarefree(const std::vector<Degree>&,coeffs,size_t,bool&);
}

// Sparse multigraded Lyubeznik resolution and homogeneous unit cancellation.
//
// A face i_1 < ... < i_p is retained exactly when no j < i_h divides
// lcm(m_i_h,...,m_i_p), for every h.  This is the L-admissibility criterion
// of G. Lyubeznik, J. Pure Appl. Algebra 51 (1988), 193--195.  Extending
// admissible suffixes by a new smallest vertex tests only the new suffix;
// a rejected suffix cannot occur in any admissible extension.  Admissible
// faces form a simplicial subcomplex, and their signed Taylor differential
// is a free resolution of the quotient by the input monomial ideal.
//
// Entries are stored as coefficients alone: the monomial of an entry is
// determined by the difference between its column and row multidegrees.
// Thus an entry is a unit precisely when these multidegrees agree.  A unit
// pivot splits off a contractible two-term free summand.  Repeating until
// no such pivot remains gives a minimal resolution over a field, without
// constructing a polynomial or computing a Groebner basis.
//
// The admissible complex can still have exponentially many faces.  The
// budget bounds constructed cells and stored differential entries, each
// independently; it also guards fill-in during cancellation.  Exceeding
// it is an explicit failure, never a truncated resolution.



#include <algorithm>
#include <climits>
#include <deque>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace monres
{
namespace
{

typedef std::vector<int> Face;

struct FaceHash
{
  size_t operator()(const Face& face) const
  {
    size_t value = face.size();
    for (Face::const_iterator it = face.begin(); it != face.end(); ++it)
      value ^= static_cast<size_t>(*it) + static_cast<size_t>(0x9e3779b9U)
               + (value << 6) + (value >> 2);
    return value;
  }
};

typedef std::unordered_map<Face, int, FaceHash> FaceIndices;

// Squarefree dispatch may retry a candidate Lyubeznik construction with the
// support-lattice algorithm when only its combinatorial budget is exhausted.
// Other failures, including interruption and arithmetic errors, must escape.
class LyubeznikBudgetExceeded : public std::runtime_error
{
public:
  explicit LyubeznikBudgetExceeded(const char* message)
    : std::runtime_error(message) {}
};

static bool divides(const Degree& divisor, const Degree& multiple)
{
  for (size_t variable = 0; variable < divisor.size(); ++variable)
    if (divisor[variable] > multiple[variable]) return false;
  return true;
}

static void consume(size_t& count, size_t budget, const char* message)
{
  if (siCntrlc) throw std::runtime_error("monres interrupted");
  if (count >= budget) throw LyubeznikBudgetExceeded(message);
  ++count;
}

static void enqueue(int column, std::deque<int>& candidates,
                    std::vector<unsigned char>& queued)
{
  if (!queued[column])
  {
    queued[column] = 1;
    candidates.push_back(column);
  }
}

// Clear all homogeneous unit pivots in one differential.  Incidence sets
// visit only columns containing the pivot row.  Rows belonging to cells
// cancelled in the preceding differential are discarded on entry.
static void cancelLayer(Complex& complex, size_t layer,
                        std::vector<std::vector<unsigned char> >& alive,
                        coeffs field, size_t budget, size_t& entryCount)
{
  std::vector<Cell>& columns = complex[layer];
  std::vector<Cell>& rows = complex[layer - 1];
  std::vector<std::set<int> > incidence(rows.size());
  std::deque<int> candidates;
  std::vector<unsigned char> queued(columns.size(), 0);

  for (size_t column = 0; column < columns.size(); ++column)
  {
    if (!alive[layer][column]) continue;
    Column& boundary = columns[column].boundary;
    for (Column::iterator term = boundary.begin(); term != boundary.end();)
    {
      const int row = term->first;
      if (!alive[layer - 1][row])
      {
        boundary.erase(term++);
        --entryCount;
      }
      else
      {
        incidence[row].insert(static_cast<int>(column));
        if (columns[column].degree == rows[row].degree)
          enqueue(static_cast<int>(column), candidates, queued);
        ++term;
      }
    }
  }

  while (!candidates.empty())
  {
    if (siCntrlc) throw std::runtime_error("monres interrupted");
    const int pivotColumn = candidates.front();
    candidates.pop_front();
    queued[pivotColumn] = 0;
    if (!alive[layer][pivotColumn]) continue;

    Column& pivot = columns[pivotColumn].boundary;
    int pivotRow = -1;
    size_t smallestIncidence = columns.size() + 1;
    for (Column::const_iterator term = pivot.begin(); term != pivot.end(); ++term)
    {
      const int row = term->first;
      if (columns[pivotColumn].degree == rows[row].degree
          && incidence[row].size() < smallestIncidence)
      {
        pivotRow = row;
        smallestIncidence = incidence[row].size();
      }
    }
    if (pivotRow < 0) continue;

    const Scalar unit = pivot.find(pivotRow)->second;
    // Copy the incident indices: sparse updates mutate the incidence sets.
    const std::vector<int> targets(incidence[pivotRow].begin(),
                                   incidence[pivotRow].end());
    for (size_t targetIndex = 0; targetIndex < targets.size(); ++targetIndex)
    {
      const int targetColumn = targets[targetIndex];
      if (targetColumn == pivotColumn) continue;
      Column& target = columns[targetColumn].boundary;
      const Scalar factor = target.find(pivotRow)->second / unit;
      for (Column::const_iterator term = pivot.begin(); term != pivot.end(); ++term)
      {
        const int row = term->first;
        const Scalar correction = factor * term->second;
        Column::iterator previous = target.find(row);
        if (previous == target.end())
        {
          const Scalar value = Scalar(0, field) - correction;
          if (!value.isZero())
          {
            consume(entryCount, budget,
                    "monomial resolution exceeded its sparse entry budget during cancellation");
            target.insert(std::make_pair(row, value));
            incidence[row].insert(targetColumn);
            if (columns[targetColumn].degree == rows[row].degree)
              enqueue(targetColumn, candidates, queued);
          }
        }
        else
        {
          const Scalar value = previous->second - correction;
          if (value.isZero())
          {
            target.erase(previous);
            incidence[row].erase(targetColumn);
            --entryCount;
          }
          else previous->second = value;
        }
      }
    }

    for (Column::const_iterator term = pivot.begin(); term != pivot.end(); ++term)
      incidence[term->first].erase(pivotColumn);
    entryCount -= pivot.size();
    pivot.clear();
    alive[layer][pivotColumn] = 0;
    alive[layer - 1][pivotRow] = 0;
    entryCount -= rows[pivotRow].boundary.size();
    rows[pivotRow].boundary.clear();

    // After clearing the pivot row, the chain condition makes the pivot
    // coordinate of the next differential zero in the new basis.  Its old
    // stored row can simply be dropped when that layer is processed.  The
    // remaining coordinates of the adjacent maps do not change.
  }
}

static void minimize(Complex& complex, coeffs field, size_t budget,
                     size_t& entryCount)
{
  std::vector<std::vector<unsigned char> > alive(complex.size());
  for (size_t layer = 0; layer < complex.size(); ++layer)
    alive[layer].assign(complex[layer].size(), 1);

  // Earlier maps subsequently lose only columns, so no new unit can
  // appear there.  A single ascending pass therefore suffices.
  for (size_t layer = 2; layer < complex.size(); ++layer)
    cancelLayer(complex, layer, alive, field, budget, entryCount);

  std::vector<std::vector<int> > renumber(complex.size());
  for (size_t layer = 0; layer < complex.size(); ++layer)
  {
    renumber[layer].assign(complex[layer].size(), -1);
    int next = 0;
    for (size_t cell = 0; cell < complex[layer].size(); ++cell)
      if (alive[layer][cell]) renumber[layer][cell] = next++;
  }

  for (size_t layer = 0; layer < complex.size(); ++layer)
  {
    std::vector<Cell> compact;
    for (size_t cell = 0; cell < complex[layer].size(); ++cell)
    {
      if (!alive[layer][cell]) continue;
      Cell next;
      next.degree.swap(complex[layer][cell].degree);
      if (layer > 0)
        for (Column::const_iterator term = complex[layer][cell].boundary.begin();
             term != complex[layer][cell].boundary.end(); ++term)
        {
          const int row = renumber[layer - 1][term->first];
          if (row >= 0) next.boundary.insert(std::make_pair(row, term->second));
        }
      compact.push_back(std::move(next));
    }
    complex[layer].swap(compact);
  }
  while (complex.size() > 1 && complex.back().empty()) complex.pop_back();
}

} // anonymous namespace

Complex lyubeznik(const std::vector<Degree>& generators, coeffs field,
                  size_t budget)
{
  if (budget == 0)
    throw std::runtime_error("monomial resolution budget must be positive");
  if (generators.size() >= static_cast<size_t>(INT_MAX))
    throw std::runtime_error("too many monomial generators for resolution indices");
  const size_t variables = generators.empty() ? 0 : generators.front().size();
  for (size_t generator = 0; generator < generators.size(); ++generator)
  {
    if (generators[generator].size() != variables)
      throw std::runtime_error("inconsistent monomial exponent-vector lengths");
    for (size_t variable = 0; variable < variables; ++variable)
      if (generators[generator][variable] < 0)
        throw std::runtime_error("negative monomial exponent in resolution input");
  }

  Complex result(1);
  Cell augmentation;
  augmentation.degree.assign(variables, 0);
  result[0].push_back(std::move(augmentation));
  size_t cellCount = 1;
  size_t entryCount = 0;
  std::vector<Face> previousFaces(1);

  for (size_t layer = 1; !previousFaces.empty(); ++layer)
  {
    FaceIndices previousIndices;
    previousIndices.reserve(previousFaces.size());
    for (size_t index = 0; index < previousFaces.size(); ++index)
      previousIndices.insert(std::make_pair(previousFaces[index], static_cast<int>(index)));

    std::vector<Face> nextFaces;
    std::vector<Cell> nextCells;
    for (size_t suffix = 0; suffix < previousFaces.size(); ++suffix)
    {
      const Face& previous = previousFaces[suffix];
      const int first = previous.empty() ? static_cast<int>(generators.size())
                                        : previous.front();
      for (int generator = 0; generator < first; ++generator)
      {
        Degree degree = result[layer - 1][suffix].degree;
        for (size_t variable = 0; variable < variables; ++variable)
          degree[variable] = std::max(degree[variable], generators[generator][variable]);
        bool admissible = true;
        for (int earlier = 0; earlier < generator; ++earlier)
          if (divides(generators[earlier], degree))
          {
            admissible = false;
            break;
          }
        if (!admissible) continue;
        consume(cellCount, budget, "monomial resolution exceeded its cell budget");
        if (nextCells.size() >= static_cast<size_t>(INT_MAX))
          throw std::runtime_error("too many cells for monomial resolution indices");

        Face face;
        face.reserve(previous.size() + 1);
        face.push_back(generator);
        face.insert(face.end(), previous.begin(), previous.end());
        Cell cell;
        cell.degree.swap(degree);
        Face facet;
        facet.reserve(previous.size());
        for (size_t removed = 0; removed < face.size(); ++removed)
        {
          facet.clear();
          facet.insert(facet.end(), face.begin(), face.begin() + removed);
          facet.insert(facet.end(), face.begin() + removed + 1, face.end());
          const FaceIndices::const_iterator row = previousIndices.find(facet);
          if (row == previousIndices.end())
            throw std::runtime_error("internal error: missing admissible Lyubeznik facet");
          consume(entryCount, budget, "monomial resolution exceeded its sparse entry budget");
          cell.boundary.insert(std::make_pair(row->second,
                               Scalar((removed % 2 == 0) ? 1 : -1, field)));
        }
        nextFaces.push_back(std::move(face));
        nextCells.push_back(std::move(cell));
      }
    }
    if (nextCells.empty()) break;
    result.push_back(std::move(nextCells));
    previousFaces.swap(nextFaces);
  }

  minimize(result, field, budget, entryCount);
  return result;
}

} // namespace monres



#include <algorithm>
#include <cstdint>
#include <limits>
#include <set>
#include <stdexcept>
#include <utility>

namespace monres
{
namespace squarefree_detail
{

// A support is a variable set, not a set of ideal generators.  In particular,
// dense squarefree ideals do not require enumerating 2^(number of generators).
typedef std::vector<std::uint64_t> Support;

Support supportOf(const Degree& degree)
{
  Support result((degree.size() + 63) / 64, 0);
  for (std::size_t i = 0; i < degree.size(); ++i)
  {
    if (degree[i] != 0 && degree[i] != 1)
      throw std::runtime_error("squarefree resolution requires squarefree generators");
    if (degree[i] != 0)
      result[i / 64] |= std::uint64_t(1) << (i % 64);
  }
  return result;
}

bool contained(const Support& small, const Support& large)
{
  for (std::size_t i = 0; i < small.size(); ++i)
    if ((small[i] & ~large[i]) != 0) return false;
  return true;
}

std::size_t cardinality(const Support& support)
{
  std::size_t result = 0;
  for (std::size_t i = 0; i < support.size(); ++i)
  {
    std::uint64_t word = support[i];
    while (word != 0)
    {
      word &= word - 1;
      ++result;
    }
  }
  return result;
}

Degree degreeOf(const Support& support, std::size_t variables)
{
  Degree result(variables, 0);
  for (std::size_t i = 0; i < variables; ++i)
    result[i] = (support[i / 64] >> (i % 64)) & 1;
  return result;
}

struct SupportOrder
{
  bool operator()(const Support& left, const Support& right) const
  {
    const std::size_t a = cardinality(left);
    const std::size_t b = cardinality(right);
    return a != b ? a < b : left < right;
  }
};

// Account for simultaneously stored coefficients, including copies used by
// echelon bases and lifted columns.  Scalar arithmetic itself has O(1) live
// temporaries; large coefficient bit sizes are controlled by the coefficient
// field rather than this combinatorial limit.
struct SparseBudget
{
  std::size_t live;
  std::size_t limit;

  SparseBudget(std::size_t initial, std::size_t maximum)
    : live(initial), limit(maximum)
  {
    if (live > limit)
      throw std::runtime_error("squarefree resolution coefficient storage exceeds budget");
  }

  void grow(std::size_t count)
  {
    if (siCntrlc) throw std::runtime_error("monres interrupted");
    if (count > limit - live)
      throw std::runtime_error("squarefree resolution coefficient fill-in exceeds budget");
    live += count;
  }

  void release(std::size_t count) { live -= count; }
};

void addScaled(Column& target, const Column& source, const Scalar& factor,
               SparseBudget& storage)
{
  if (factor.isZero()) return;
  for (Column::const_iterator it = source.begin(); it != source.end(); ++it)
  {
    const Scalar term = it->second * factor;
    if (term.isZero()) continue;
    Column::iterator old = target.find(it->first);
    if (old == target.end())
    {
      storage.grow(1);
      target.insert(std::make_pair(it->first, term));
    }
    else
    {
      const Scalar sum = old->second + term;
      if (sum.isZero())
      {
        target.erase(old);
        storage.release(1);
      }
      else old->second = sum;
    }
  }
}

void normalize(Column& column, const Scalar& pivot)
{
  for (Column::iterator it = column.begin(); it != column.end(); ++it)
    it->second = it->second / pivot;
}

// Column echelon form over the actual coefficient field.  No integer ranks
// are cached: the construction must also detect characteristic-dependent
// squarefree Betti numbers.
typedef std::map<int, Column> Echelon;

void reduce(Column& vector, const Echelon& basis, const Scalar& zero,
            SparseBudget& storage)
{
  while (!vector.empty())
  {
    const Echelon::const_iterator pivot = basis.find(vector.begin()->first);
    if (pivot == basis.end()) return;
    const Scalar factor = zero - vector.begin()->second;
    addScaled(vector, pivot->second, factor, storage);
  }
}

bool insert(Column& vector, Echelon& basis, const Scalar& zero,
            SparseBudget& storage)
{
  reduce(vector, basis, zero, storage);
  if (vector.empty()) return false;
  const Scalar pivot = vector.begin()->second;
  normalize(vector, pivot);
  storage.grow(vector.size());
  basis.insert(std::make_pair(vector.begin()->first, vector));
  return true;
}

struct LiftedColumn
{
  Column image;
  Column preimage;
};

typedef std::map<int, LiftedColumn> LiftedEchelon;

void reduceLifted(Column& image, Column& preimage,
                  const LiftedEchelon& basis, const Scalar& zero,
                  SparseBudget& storage)
{
  while (!image.empty())
  {
    const LiftedEchelon::const_iterator pivot = basis.find(image.begin()->first);
    if (pivot == basis.end()) return;
    const Scalar factor = zero - image.begin()->second;
    addScaled(image, pivot->second.image, factor, storage);
    addScaled(preimage, pivot->second.preimage, factor, storage);
  }
}

std::vector<Support> supportLattice(const std::vector<Support>& generators,
                                    std::size_t words, std::size_t budget)
{
  std::vector<Support> result(1, Support(words, 0));
  std::set<Support> seen;
  seen.insert(result[0]);
  for (std::size_t i = 0; i < generators.size(); ++i)
  {
    if (siCntrlc) throw std::runtime_error("monres interrupted");
    const std::size_t oldSize = result.size();
    for (std::size_t j = 0; j < oldSize; ++j)
    {
      Support joined(result[j]);
      for (std::size_t k = 0; k < words; ++k) joined[k] |= generators[i][k];
      if (seen.insert(joined).second)
      {
        if (result.size() >= budget)
          throw std::runtime_error("squarefree resolution support lattice exceeds budget");
        result.push_back(joined);
      }
    }
  }
  std::sort(result.begin(), result.end(), SupportOrder());
  return result;
}

bool preferLyubeznik(const std::vector<Support>& generators)
{
  bool atMostQuadratic = true;
  bool pairwiseDisjoint = true;
  Support used(generators[0].size(), 0);
  for (std::size_t i = 0; i < generators.size(); ++i)
  {
    if (cardinality(generators[i]) > 2) atMostQuadratic = false;
    for (std::size_t word = 0; word < used.size(); ++word)
    {
      if ((used[word] & generators[i][word]) != 0) pairwiseDisjoint = false;
      used[word] |= generators[i][word];
    }
  }
  return atMostQuadratic || pairwiseDisjoint;
}

} // namespace squarefree_detail

Complex squarefree(const std::vector<Degree>& generators,
                   coeffs field, std::size_t budget,
                   bool& usedSupportLattice)
{
  using namespace squarefree_detail;
  usedSupportLattice = true;
  const std::size_t variables = generators.empty() ? 0 : generators[0].size();
  if (budget == 0 || generators.size() >= budget)
    throw std::runtime_error("squarefree resolution cells exceed budget");
  if (generators.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
    throw std::runtime_error("squarefree resolution has too many generators");

  std::vector<Support> generatorSupports;
  generatorSupports.reserve(generators.size());
  for (std::size_t i = 0; i < generators.size(); ++i)
  {
    if (generators[i].size() != variables)
      throw std::runtime_error("inconsistent generator degree vectors");
    generatorSupports.push_back(supportOf(generators[i]));
  }

  // Quadratic squarefree ideals are much cheaper as Lyubeznik faces and
  // cancellation than as repeated support-homology problems.  The same holds
  // for disjoint supports, where the Lyubeznik/Taylor complex is already
  // minimal.  If that construction reaches the caller's budget, retain the
  // support algorithm so dispatch does not turn a formerly successful input
  // into a budget failure.
  if (generators.size() > 1 && preferLyubeznik(generatorSupports))
  {
    try
    {
      Complex candidate = lyubeznik(generators, field, budget);
      usedSupportLattice = false;
      return candidate;
    }
    catch (const LyubeznikBudgetExceeded&)
    {
    }
  }

  const Scalar zero(0, field);
  const Scalar one(1, field);
  Complex result(1);
  Cell ringCell;
  ringCell.degree = Degree(variables, 0);
  result[0].push_back(ringCell);
  if (generators.empty()) return result;

  result.push_back(std::vector<Cell>());
  for (std::size_t i = 0; i < generators.size(); ++i)
  {
    Cell generator;
    generator.degree = generators[i];
    generator.boundary.insert(std::make_pair(0, one));
    result[1].push_back(generator);
  }

  // Minimal input containing 1 consists of that generator alone.  Principal
  // ideals likewise already have their complete free resolution.
  if (generators.size() == 1) return result;

  const std::vector<Support> lattice = supportLattice(
    generatorSupports, (variables + 63) / 64, budget);
  std::vector<Support> sourceSupports = generatorSupports;
  std::size_t cells = generators.size() + 1;
  std::size_t persistentEntries = generators.size();

  // F_0 = R.  At degree alpha, homogenization identifies (F_i)_alpha
  // with a vector space on cells whose supports lie in alpha, and identifies
  // its differential with the stored coefficient columns.  Extend the
  // boundaries already generated in smaller supports to a basis of the
  // kernel.  These complements are precisely minimal new syzygy generators.
  // Taylor's resolution shows that all required degrees lie in the lcm
  // lattice; Hilbert's syzygy theorem bounds the final index by variables.
  for (std::size_t level = 2; level <= variables; ++level)
  {
    const std::vector<Cell>& sources = result[level - 1];
    std::vector<Cell> next;
    std::vector<Support> nextSupports;
    for (std::size_t a = 0; a < lattice.size(); ++a)
    {
      if (siCntrlc) throw std::runtime_error("monres interrupted");
      const Support& alpha = lattice[a];
      if (cardinality(alpha) < level) continue;

      std::vector<int> active;
      for (std::size_t j = 0; j < sources.size(); ++j)
        if (contained(sourceSupports[j], alpha)) active.push_back(static_cast<int>(j));
      if (active.size() < 2) continue;

      SparseBudget storage(persistentEntries, budget);
      Echelon boundaries;
      for (std::size_t j = 0; j < next.size(); ++j)
        if (contained(nextSupports[j], alpha))
        {
          storage.grow(next[j].boundary.size());
          Column boundary(next[j].boundary);
          insert(boundary, boundaries, zero, storage);
          storage.release(boundary.size());
        }

      // Pivot positions of the boundary echelon basis can be omitted: the
      // other coordinate vectors span a complement Q to its image B.  Since
      // B consists of cycles, ker(D|Q) is the required quotient ker(D)/B.
      std::vector<int> complement;
      for (std::size_t j = 0; j < active.size(); ++j)
        if (boundaries.find(active[j]) == boundaries.end())
          complement.push_back(active[j]);
      if (complement.size() < 2) continue;

      LiftedEchelon imageBasis;
      for (std::size_t j = 0; j < complement.size(); ++j)
      {
        storage.grow(sources[complement[j]].boundary.size() + 1);
        Column image(sources[complement[j]].boundary);
        Column preimage;
        preimage.insert(std::make_pair(complement[j], one));
        reduceLifted(image, preimage, imageBasis, zero, storage);
        if (!image.empty())
        {
          const int row = image.begin()->first;
          const Scalar pivot = image.begin()->second;
          normalize(image, pivot);
          normalize(preimage, pivot);
          LiftedColumn& lifted = imageBasis.insert(
            std::make_pair(row, LiftedColumn())).first->second;
          lifted.image.swap(image);
          lifted.preimage.swap(preimage);
        }
        else if (insert(preimage, boundaries, zero, storage))
        {
          if (cells >= budget ||
              next.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::runtime_error("squarefree resolution cells exceed budget");
          ++cells;
          persistentEntries += preimage.size();
          Cell syzygy;
          syzygy.degree = degreeOf(alpha, variables);
          syzygy.boundary.swap(preimage);
          next.push_back(Cell());
          next.back().degree.swap(syzygy.degree);
          next.back().boundary.swap(syzygy.boundary);
          nextSupports.push_back(alpha);
        }
      }
    }
    if (next.empty()) break;
    result.push_back(std::vector<Cell>());
    result.back().swap(next);
    sourceSupports.swap(nextSupports);
  }
  return result;
}

} // namespace monres

// Fast path for regular linear quotients: Herzog--Takayama, "Resolutions by
// mapping cones", HHA 4(2) (2002), Lemma 1.5, Definition 1.9, Theorem 1.12.
// Primary source: https://emis.de/ft/51840
// This fragment follows the shared Scalar/Cell/Complex declarations.
#include <algorithm>
#include <climits>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

namespace monres {
namespace regular_detail {

using Subset = std::vector<int>;

struct Data {
  std::vector<Subset> sets;
  std::vector<std::map<int, int> > decomposition;
  size_t maxSet = 0;
  size_t cells = 0;
  size_t entries = 0;
};

static void addCount(size_t& value,size_t add,size_t budget,const char* message) {
  if(add>budget-value) throw std::runtime_error(message);
  value+=add;
}

static size_t powerOfTwo(size_t exponent,size_t limit,const char* message) {
  size_t value=1;
  for(size_t i=0;i<exponent;++i) {
    if(value>limit/2) throw std::runtime_error(message);
    value*=2;
  }
  return value;
}

} // namespace regular_detail

bool regularQuotientData(const std::vector<Degree>& generators,
                         size_t budget, regular_detail::Data& result) {
  using namespace regular_detail;
  if (generators.empty()) return false;
  const size_t variables = generators.front().size();
  const size_t count = generators.size();
  if (count >= static_cast<size_t>(INT_MAX) || variables > static_cast<size_t>(INT_MAX))
    throw std::runtime_error("regular quotient resolution exceeds index range");
  if (budget == 0 || count >= budget)
    throw std::runtime_error("regular quotient resolution exceeds cell budget");

  // Nondecreasing generator degrees are required for minimality.  Check this
  // rather than relying on the caller's sorting convention.  An overflow in
  // this optional recognition path is a failure to recognize, not a result.
  unsigned long long previousDegree = 0;
  for (size_t j = 0; j < count; ++j) {
    if (generators[j].size() != variables) return false;
    unsigned long long total = 0;
    for (long exponent : generators[j]) {
      if (exponent < 0 || static_cast<unsigned long long>(exponent) >
          std::numeric_limits<unsigned long long>::max() - total) return false;
      total += static_cast<unsigned long long>(exponent);
    }
    if (total == 0 || total < previousDegree) return false;
    previousDegree = total;
  }

  std::vector<Subset> sets(count);
  std::vector<std::map<int, int>> decomposition(count);
  for (size_t j = 1; j < count; ++j) {
    if(siCntrlc) throw std::runtime_error("monres interrupted");
    // q_i=u_i/gcd(u_i,u_j) generates the colon.  Record every q_i=x_s;
    // its earliest occurrence is exactly g(x_s*u_j), since the input is
    // minimal and hence no q_i is 1.
    for (size_t i = 0; i < j; ++i) {
      int singleton = -1;
      bool nonlinear = false;
      for (size_t v = 0; v < variables; ++v) {
        if (generators[i][v] <= generators[j][v]) continue;
        const long difference = generators[i][v] - generators[j][v];
        if (difference != 1 || singleton >= 0) { nonlinear = true; break; }
        singleton = static_cast<int>(v);
      }
      if (!nonlinear && singleton < 0) return false; // nonminimal input
      if (!nonlinear) decomposition[j].insert(std::make_pair(singleton, static_cast<int>(i)));
    }
    for (const auto& term : decomposition[j]) sets[j].push_back(term.first);

    // The colon is generated by variables iff every q_i is divisible by at
    // least one of the variable generators just collected.
    for (size_t i = 0; i < j; ++i) {
      bool covered = false;
      for (int s : sets[j])
        if (generators[i][s] > generators[j][s]) { covered = true; break; }
      if (!covered) return false;
    }

    // This is precisely regularity of the earliest-divisor decomposition
    // function.  Linear quotients alone do not justify the explicit formula.
    for (const auto& term : decomposition[j]) {
      const Subset& target = sets[term.second];
      if (!std::includes(sets[j].begin(), sets[j].end(), target.begin(), target.end()))
        return false;
    }
    for(int variable:sets[j])
      if(generators[j][variable]==std::numeric_limits<long>::max())
        throw std::runtime_error("exponent overflow in regular quotient resolution");
  }

  // Exact cell count: 1 + sum_j 2^|set(u_j)|.  Check by bounded doubling,
  // avoiding a machine-word limitation on the number of variables.
  size_t cells = 1;
  size_t maxSet = 0;
  for (const Subset& set : sets) {
    size_t subsets = 1;
    for (size_t k = 0; k < set.size(); ++k) {
      if (subsets > (budget - cells) / 2)
        throw std::runtime_error("regular quotient resolution exceeds cell budget");
      subsets *= 2;
    }
    if (subsets > budget - cells)
      throw std::runtime_error("regular quotient resolution exceeds cell budget");
    cells += subsets;
    maxSet = std::max(maxSet, set.size());
  }

  // Count formula terms without constructing a single basis cell.  The first
  // summand contributes q*2^(q-1) terms for a q-variable colon set.  For the
  // decomposition term indexed by s, the facet is an arbitrary subset of
  // set(g(s*u_j)) not containing s.
  size_t entries=count;
  for(size_t j=0;j<count;++j) {
    const size_t q=sets[j].size();
    if(q!=0) {
      const size_t half=powerOfTwo(q-1,budget,
        "regular quotient resolution exceeds sparse entry budget");
      if(q>budget/half) throw std::runtime_error(
        "regular quotient resolution exceeds sparse entry budget");
      addCount(entries,q*half,budget,
        "regular quotient resolution exceeds sparse entry budget");
    }
    for(const auto& term:decomposition[j]) {
      size_t exponent=sets[term.second].size();
      if(std::binary_search(sets[term.second].begin(),sets[term.second].end(),term.first))
        --exponent;
      addCount(entries,powerOfTwo(exponent,budget,
          "regular quotient resolution exceeds sparse entry budget"),budget,
        "regular quotient resolution exceeds sparse entry budget");
    }
  }
  result.sets.swap(sets);
  result.decomposition.swap(decomposition);
  result.maxSet=maxSet;
  result.cells=cells;
  result.entries=entries;
  return true;
}

} // namespace monres

#include "Singular/libsingular.h"
#include "Singular/blackbox.h"
#include "Singular/mod_lib.h"
#include "kernel/GBEngine/syz.h"
#include "misc/options.h"
#include <algorithm>
#include <cstdint>
#include <memory>
#include <sstream>
#include <limits>

namespace monres {
namespace {

// The builders use editable sparse maps. Finished objects use compressed sparse
// columns, with one shared coefficient field and no polynomial or map objects.
// A squarefree label takes ceil(nvars/64) words rather than nvars exponents.
// Regular-linear-quotient resolutions are smaller still: their maps are kept
// as the Herzog--Takayama formula, not as enumerated sparse columns.

struct RegularFormula {
  bool squarefree = false;
  size_t variables = 0;
  size_t generators = 0;
  size_t entries = 0;
  std::vector<long> generatorDegrees;
  std::vector<uint64_t> generatorSupports;
  std::vector<size_t> colonStarts;
  std::vector<int> colonVariables;
  std::vector<int> decomposition;
  std::vector<std::vector<size_t> > combinations;
  // offsets[p][j] is the first cell belonging to generator j in F_p.
  std::vector<std::vector<size_t> > offsets;

  size_t words() const { return (variables+63)/64; }
  size_t colonSize(size_t j) const { return colonStarts[j+1]-colonStarts[j]; }
  size_t choose(size_t n,size_t k) const {
    return k<=n && n<combinations.size() ? combinations[n][k] : 0;
  }
  long generatorExponent(size_t j,size_t v) const {
    if(!squarefree) return generatorDegrees[j*variables+v];
    return (generatorSupports[j*words()+v/64]>>(v%64))&1;
  }
  std::pair<size_t,size_t> ownerAndLocal(size_t p,size_t column) const {
    if(p==0 || p>=offsets.size() || column>=offsets[p].back())
      throw std::runtime_error("regular quotient cell outside its resolution");
    const std::vector<size_t>& level=offsets[p];
    const auto stop=std::upper_bound(level.begin(),level.end(),column);
    const size_t j=static_cast<size_t>(stop-level.begin()-1);
    return std::make_pair(j,column-level[j]);
  }
  std::vector<size_t> subset(size_t p,size_t column) const {
    const std::pair<size_t,size_t> owner=ownerAndLocal(p,column);
    const size_t n=colonSize(owner.first),k=p-1;
    size_t rank=owner.second,candidate=0;
    std::vector<size_t> result;
    result.reserve(k);
    for(size_t i=0;i<k;++i) {
      for(;;++candidate) {
        const size_t block=choose(n-candidate-1,k-i-1);
        if(rank<block) { result.push_back(candidate++); break; }
        rank-=block;
      }
    }
    return result;
  }
};

struct PackedLayer {
  size_t rank = 0;
  std::vector<long> degrees;
  std::vector<uint64_t> supports;
  std::vector<size_t> starts;
  std::vector<int> rows;
  // A layer whose coefficients are all +1/-1 stores one bit per entry.
  // Otherwise values owns the general coefficient-field elements.  This is
  // deliberately a per-layer choice: generic elimination may produce
  // arbitrary scalars in one map while another map remains signed.
  bool signedCoefficients = false;
  std::vector<uint64_t> negative;
  std::vector<number> values;
};
struct PackedComplex {
  ring owner;
  bool squarefree;
  const char* method;
  std::vector<PackedLayer> layers;
  std::unique_ptr<RegularFormula> regular;
  PackedComplex(ring r, bool sf,const char* algorithm): owner(rIncRefCnt(r)), squarefree(sf), method(algorithm) {}
  PackedComplex(const PackedComplex&) = delete;
  PackedComplex& operator=(const PackedComplex&) = delete;
  ~PackedComplex() {
    for (auto& l: layers)
      for (auto& n: l.values) n_Delete(&n,owner->cf);
    rKill(owner);
  }
  long exponent(size_t p,size_t c,int v) const {
    if(regular) {
      if(p==0) return 0;
      const std::pair<size_t,size_t> owner=regular->ownerAndLocal(p,c);
      const size_t j=owner.first;
      bool selected=false;
      for(size_t candidate:regular->subset(p,c)) {
        if(static_cast<size_t>(regular->colonVariables[regular->colonStarts[j]+candidate])
           ==static_cast<size_t>(v)) selected=true;
      }
      const long base=regular->generatorExponent(j,v);
      if(selected && base==std::numeric_limits<long>::max())
        throw std::runtime_error("exponent overflow in regular quotient resolution");
      return base+(selected?1:0);
    }
    const PackedLayer& l=layers[p];
    if (!squarefree) return l.degrees[c*owner->N+v];
    const size_t words=(owner->N+63)/64;
    return (l.supports[c*words+v/64]>>(v%64))&1;
  }
  size_t entries() const {
    if(regular) return regular->entries;
    size_t n=0; for (const auto& l:layers) n+=l.rows.size(); return n;
  }
  size_t payloadBytes() const {
    size_t n=0;
    if(regular) {
      n+=regular->generatorDegrees.size()*sizeof(long)
        +regular->generatorSupports.size()*sizeof(uint64_t)
        +regular->colonStarts.size()*sizeof(size_t)
        +regular->colonVariables.size()*sizeof(int)
        +regular->decomposition.size()*sizeof(int);
      for(const auto& offset:regular->offsets) n+=offset.size()*sizeof(size_t);
      for(const auto& row:regular->combinations) n+=row.size()*sizeof(size_t);
    }
    for (const auto& l:layers)
      n+=l.degrees.size()*sizeof(long)+l.supports.size()*sizeof(uint64_t)
        +l.starts.size()*sizeof(size_t)+l.rows.size()*sizeof(int)
        +l.negative.size()*sizeof(uint64_t)+l.values.size()*sizeof(number);
    return n;
  }
};
using Box=std::shared_ptr<const PackedComplex>;
static int boxType=0;

static Box pack(Complex& c,ring r,bool sf,const char* method) {
  std::shared_ptr<PackedComplex> b(new PackedComplex(r,sf,method));
  b->layers.reserve(c.size());
  for (auto& level:c) {
    b->layers.emplace_back(); PackedLayer& l=b->layers.back();
    l.rank=level.size();
    const size_t words=(r->N+63)/64;
    if(sf) l.supports.resize(l.rank*words,0);
    else l.degrees.reserve(l.rank*r->N);
    size_t nnz=0; for(const auto& cell:level) nnz+=cell.boundary.size();
    l.signedCoefficients=true;
    for(const auto& cell:level) for(const auto& term:cell.boundary)
      if(!n_IsOne(term.second.value,r->cf) && !n_IsMOne(term.second.value,r->cf))
        l.signedCoefficients=false;
    l.rows.reserve(nnz); l.starts.reserve(l.rank+1);
    if(l.signedCoefficients) l.negative.assign((nnz+63)/64,0);
    else l.values.reserve(nnz);
    l.starts.push_back(0);
    for(size_t j=0;j<level.size();++j) {
      if(sf) {
        for(int v=0;v<r->N;++v)
          if(level[j].degree[v]) l.supports[j*words+v/64]|=uint64_t(1)<<(v%64);
      } else l.degrees.insert(l.degrees.end(),level[j].degree.begin(),level[j].degree.end());
      for(auto& term:level[j].boundary) {
        const size_t k=l.rows.size();
        l.rows.push_back(term.first);
        if(l.signedCoefficients) {
          // In characteristic two, -1 equals +1 and n_IsOne wins.
          if(!n_IsOne(term.second.value,r->cf)) l.negative[k/64]|=uint64_t(1)<<(k%64);
        } else {
          l.values.push_back(term.second.value);
          term.second.field=NULL; // Transfer coefficient ownership, without copy.
        }
      }
      l.starts.push_back(l.rows.size());
    }
    std::vector<Cell>().swap(level);
  }
  return b;
}

static Box packRegular(const std::vector<Degree>& generators,
                       const regular_detail::Data& data,ring r,bool sf) {
  std::shared_ptr<PackedComplex> b(new PackedComplex(
    r,sf,"regular linear quotients (formula)"));
  b->regular.reset(new RegularFormula());
  RegularFormula& f=*b->regular;
  f.squarefree=sf;
  f.variables=r->N;
  f.generators=generators.size();
  f.entries=data.entries;
  if(sf) {
    f.generatorSupports.assign(f.generators*f.words(),0);
    for(size_t j=0;j<f.generators;++j)
      for(size_t v=0;v<f.variables;++v)
        if(generators[j][v])
          f.generatorSupports[j*f.words()+v/64]|=uint64_t(1)<<(v%64);
  } else {
    f.generatorDegrees.reserve(f.generators*f.variables);
    for(const Degree& d:generators)
      f.generatorDegrees.insert(f.generatorDegrees.end(),d.begin(),d.end());
  }

  f.colonStarts.reserve(f.generators+1);
  f.colonStarts.push_back(0);
  for(size_t j=0;j<f.generators;++j) {
    for(int variable:data.sets[j]) {
      f.colonVariables.push_back(variable);
      f.decomposition.push_back(data.decomposition[j].find(variable)->second);
    }
    f.colonStarts.push_back(f.colonVariables.size());
  }

  f.combinations.resize(data.maxSet+1);
  for(size_t n=0;n<=data.maxSet;++n) {
    f.combinations[n].assign(n+1,0);
    f.combinations[n][0]=f.combinations[n][n]=1;
    for(size_t k=1;k<n;++k) {
      const size_t left=f.combinations[n-1][k-1];
      const size_t right=f.combinations[n-1][k];
      if(left>std::numeric_limits<size_t>::max()-right)
        throw std::runtime_error("regular quotient subset count exceeds addressable range");
      f.combinations[n][k]=left+right;
    }
  }

  b->layers.resize(data.maxSet+2);
  b->layers[0].rank=1;
  f.offsets.resize(b->layers.size());
  for(size_t p=1;p<b->layers.size();++p) {
    std::vector<size_t>& offset=f.offsets[p];
    offset.reserve(f.generators+1);
    offset.push_back(0);
    for(size_t j=0;j<f.generators;++j) {
      const size_t add=f.choose(f.colonSize(j),p-1);
      if(add>static_cast<size_t>(INT_MAX)-offset.back())
        throw std::runtime_error("regular quotient resolution exceeds index range");
      offset.push_back(offset.back()+add);
    }
    b->layers[p].rank=offset.back();
  }
  return b;
}

static number packedCoefficient(const PackedLayer& l,size_t k,coeffs field) {
  if(!l.signedCoefficients) return n_Copy(l.values[k],field);
  return n_Init(((l.negative[k/64]>>(k%64))&1) ? -1 : 1,field);
}

static std::vector<size_t> unrankLex(const RegularFormula& f,size_t n,size_t k,size_t rank) {
  if(rank>=f.choose(n,k)) throw std::runtime_error("invalid regular quotient subset rank");
  std::vector<size_t> positions;
  positions.reserve(k);
  size_t candidate=0;
  for(size_t i=0;i<k;++i) {
    for(;;++candidate) {
      const size_t block=f.choose(n-candidate-1,k-i-1);
      if(rank<block) { positions.push_back(candidate++); break; }
      rank-=block;
    }
  }
  return positions;
}

struct FormulaTerm {
  size_t row;
  int sign;
  int removedVariable;
  size_t sourceGenerator;
  size_t targetGenerator;
  bool augmentation;
};

static size_t rankLexSkipping(const RegularFormula& f,size_t n,const size_t* positions,size_t count,
                              size_t skipped) {
  const size_t k=count-1;
  size_t rank=0,start=0,out=0;
  for(size_t i=0;i<count;++i) if(i!=skipped) {
    const size_t p=positions[i];
    rank+=f.choose(n-start,k-out)-f.choose(n-p,k-out);
    start=p+1;
    ++out;
  }
  return rank;
}

static bool targetFacetRank(const RegularFormula& f,size_t source,size_t target,
                            const size_t* sigma,size_t count,size_t skipped,
                            size_t& rank) {
  const size_t k=count-1,n=f.colonSize(target);
  size_t search=0,start=0,out=0;
  rank=0;
  for(size_t i=0;i<count;++i) if(i!=skipped) {
    const int variable=f.colonVariables[f.colonStarts[source]+sigma[i]];
    while(search<n && f.colonVariables[f.colonStarts[target]+search]<variable)
      ++search;
    if(search==n || f.colonVariables[f.colonStarts[target]+search]!=variable)
      return false;
    rank+=f.choose(n-start,k-out)-f.choose(n-search,k-out);
    start=++search;
    ++out;
  }
  return true;
}

template<class Emit>
static void regularBoundaryKnown(const RegularFormula& f,size_t p,size_t j,
                                 const size_t* sigma,size_t sigmaSize,Emit emit) {
  if(p==1) {
    emit(FormulaTerm{0,1,-1,j,j,true});
    return;
  }
  const size_t k=p-1,n=f.colonSize(j);
  for(size_t h=0;h<k;++h) {
    const size_t aligned=f.colonStarts[j]+sigma[h];
    const int removed=f.colonVariables[aligned];
    const int sign=(h&1) ? -1 : 1;
    emit(FormulaTerm{f.offsets[p-1][j]+rankLexSkipping(f,n,sigma,sigmaSize,h),
                     sign,removed,j,j,false});

    const size_t g=static_cast<size_t>(f.decomposition[aligned]);
    size_t targetRank=0;
    if(targetFacetRank(f,j,g,sigma,sigmaSize,h,targetRank))
      emit(FormulaTerm{f.offsets[p-1][g]+targetRank,
                       -sign,removed,j,g,false});
  }
}

template<class Emit>
static void regularBoundary(const RegularFormula& f,size_t p,size_t column,Emit emit) {
  const std::pair<size_t,size_t> owner=f.ownerAndLocal(p,column);
  const std::vector<size_t> sigma=unrankLex(f,f.colonSize(owner.first),p-1,owner.second);
  regularBoundaryKnown(f,p,owner.first,sigma.data(),sigma.size(),emit);
}

template<class Emit>
static void regularCells(const RegularFormula& f,size_t p,Emit emit) {
  if(p==0 || p>=f.offsets.size()) return;
  const size_t k=p-1;
  if(k>=sizeof(size_t)*CHAR_BIT)
    throw std::runtime_error("regular quotient subset exceeds addressable cell budget");
  size_t sinceInterruptCheck=0;
  for(size_t j=0;j<f.generators;++j) {
    const size_t n=f.colonSize(j);
    if(k>n) continue;
    size_t sigma[sizeof(size_t)*CHAR_BIT];
    for(size_t h=0;h<k;++h) sigma[h]=h;
    bool more=true;
    while(more) {
      if((sinceInterruptCheck++&4095)==0 && siCntrlc)
        throw std::runtime_error("monres interrupted");
      emit(j,sigma,k);
      if(k==0) more=false;
      else {
        size_t h=k;
        while(h>0 && sigma[h-1]==n-k+h-1) --h;
        if(h==0) more=false;
        else {
          ++sigma[h-1];
          for(size_t q=h;q<k;++q) sigma[q]=sigma[q-1]+1;
        }
      }
    }
  }
}

static bool inputDivides(const Degree& a,const Degree& b) {
  for(size_t k=0;k<a.size();++k) if(a[k]>b[k]) return false;
  return true;
}
// A ring may admit 63-bit exponents in several variables.  Keep a two-word
// sum so that degree ordering (and hence minimal-generator pruning) cannot
// overflow even when the total degree exceeds a machine word.
static std::pair<unsigned long long,unsigned long long> totalDegree(const Degree& d) {
  unsigned long long high=0,low=0;
  for(long e:d) {
    const unsigned long long previous=low;
    low+=static_cast<unsigned long long>(e);
    if(low<previous) ++high;
  }
  return std::make_pair(high,low);
}
static std::vector<Degree> readGenerators(ideal I,bool sf) {
  if(currRing==NULL || currRing->qideal!=NULL || rIsNCRing(currRing)
     || !rHasGlobalOrdering(currRing) || nCoeff_is_Ring(currRing->cf)
     || rField_is_numeric(currRing))
    throw std::runtime_error("monres requires a commutative polynomial ring over an exact field with global ordering");
  std::vector<Degree> input;
  for(int j=0;j<IDELEMS(I);++j) {
    poly p=I->m[j]; if(p==NULL) continue;
    if(pNext(p)!=NULL || p_GetComp(p,currRing)!=0)
      throw std::runtime_error("monres requires a monomial ideal");
    Degree d(currRing->N);
    for(int v=0;v<currRing->N;++v) {
      d[v]=p_GetExp(p,v+1,currRing);
      if(sf && d[v]>1) throw std::runtime_error("squarefreeResolution requires squarefree monomial generators");
    }
    input.push_back(std::move(d));
  }
  // Deterministic degree/lex order makes divisibility pruning independent of the
  // user's generator order and places likely rooting generators first.
  std::sort(input.begin(),input.end(),[](const Degree& a,const Degree& b) {
    const auto da=totalDegree(a), db=totalDegree(b);
    return da!=db ? da<db : a>b;
  });
  std::vector<Degree> minimal;
  for(auto& d:input) {
    bool redundant=false;
    for(const auto& a:minimal) if(inputDivides(a,d)) { redundant=true; break; }
    if(!redundant) minimal.push_back(std::move(d));
  }
  return minimal;
}

static Box build(leftv args,bool sf) {
  if(!args || args->Typ()!=IDEAL_CMD)
    throw std::runtime_error("expected an ideal and an optional positive integer budget");
  size_t budget=200000;
  if(args->next) {
    leftv v=args->next;
    if(v->Typ()!=INT_CMD || v->next || (long)v->Data()<=0)
      throw std::runtime_error("monres budget must be a positive integer");
    budget=(size_t)(long)v->Data();
  }
  auto gens=readGenerators((ideal)args->Data(),sf);
  Complex c;
  const char* method=sf?"support lattice":"Lyubeznik";
  if(gens.empty()) {
    c.resize(1); c[0].push_back(Cell{Degree(currRing->N,0),Column()});
  } else {
    regular_detail::Data regular;
    if(regularQuotientData(gens,budget,regular))
      return packRegular(gens,regular,currRing,sf);
    if(sf) {
      bool usedSupportLattice=true;
      c=squarefree(gens,currRing->cf,budget,usedSupportLattice);
      method=usedSupportLattice?"support lattice":"squarefree Lyubeznik";
    } else c=lyubeznik(gens,currRing->cf,budget);
  }
  return pack(c,currRing,sf,method);
}
static const PackedComplex& getBox(leftv a) {
  if(!a || a->Typ()!=boxType || a->Data()==NULL || !*(Box*)a->Data())
    throw std::runtime_error("expected an initialized monres object");
  const PackedComplex& b=**(Box*)a->Data();
  if(currRing!=b.owner) throw std::runtime_error("monres object belongs to a different basering");
  return b;
}
static size_t getLevel(const PackedComplex& b,leftv a,bool allowZero=false) {
  if(!a || a->Typ()!=INT_CMD || (long)a->Data()<(allowZero?0:1)
     || (size_t)(long)a->Data()>=b.layers.size())
    throw std::runtime_error("homological degree outside the monres object");
  return (size_t)(long)a->Data();
}
static poly formulaEntry(const PackedComplex& b,const FormulaTerm& term,
                         bool vectorEntry) {
  const RegularFormula& f=*b.regular;
  poly result=p_NSet(n_Init(term.sign,b.owner->cf),b.owner);
  if(f.squarefree) {
    const size_t words=f.words();
    for(size_t w=0;w<words;++w) {
      uint64_t bits=0;
      if(term.augmentation)
        bits=f.generatorSupports[term.sourceGenerator*words+w];
      else if(term.sourceGenerator==term.targetGenerator) {
        if(static_cast<size_t>(term.removedVariable/64)==w)
          bits=uint64_t(1)<<(term.removedVariable%64);
      } else {
        bits=f.generatorSupports[term.sourceGenerator*words+w];
        if(static_cast<size_t>(term.removedVariable/64)==w)
          bits|=uint64_t(1)<<(term.removedVariable%64);
        bits&=~f.generatorSupports[term.targetGenerator*words+w];
      }
      while(bits) {
#if defined(__GNUC__) || defined(__clang__)
        const unsigned bit=static_cast<unsigned>(__builtin_ctzll(bits));
#else
        unsigned bit=0; while(((bits>>bit)&1)==0) ++bit;
#endif
        const size_t variable=w*64+bit;
        if(variable<f.variables) p_SetExp(result,variable+1,1,b.owner);
        bits&=bits-1;
      }
    }
  } else {
    for(size_t v=0;v<f.variables;++v) {
      long exponent=0;
      if(term.augmentation) exponent=f.generatorExponent(term.sourceGenerator,v);
      else if(term.sourceGenerator==term.targetGenerator)
        exponent=(v==static_cast<size_t>(term.removedVariable));
      else {
        const long source=f.generatorExponent(term.sourceGenerator,v);
        const long target=f.generatorExponent(term.targetGenerator,v);
        if(v==static_cast<size_t>(term.removedVariable)
           && source==std::numeric_limits<long>::max())
          throw std::runtime_error("exponent overflow in regular quotient differential");
        exponent=source+(v==static_cast<size_t>(term.removedVariable))-target;
        if(exponent<0) throw std::runtime_error("invalid regular quotient differential degree");
      }
      if(exponent) p_SetExp(result,v+1,exponent,b.owner);
    }
  }
  if(vectorEntry) p_SetComp(result,term.row+1,b.owner);
  p_Setm(result,b.owner);
  return result;
}

static poly entry(const PackedComplex& b,size_t p,size_t col,size_t k,bool vectorEntry) {
  const PackedLayer& l=b.layers[p];
  const int row=l.rows[k];
  poly t=p_NSet(packedCoefficient(l,k,b.owner->cf),b.owner);
  for(int v=0;v<b.owner->N;++v)
    p_SetExp(t,v+1,b.exponent(p,col,v)-b.exponent(p-1,row,v),b.owner);
  if(vectorEntry) p_SetComp(t,row+1,b.owner);
  p_Setm(t,b.owner); return t;
}
static ideal expandLayer(const PackedComplex& b,size_t p,bool firstIdeal) {
  const PackedLayer& l=b.layers[p];
  ideal out=idInit(std::max(size_t(1),l.rank),b.layers[p-1].rank);
  if(b.regular) {
    size_t column=0;
    try {
      regularCells(*b.regular,p,[&](size_t j,const size_t* sigma,size_t sigmaSize) {
        regularBoundaryKnown(*b.regular,p,j,sigma,sigmaSize,[&](const FormulaTerm& term) {
          out->m[column]=p_Add_q(out->m[column],formulaEntry(b,term,!firstIdeal),b.owner);
        });
        ++column;
      });
    } catch(...) {
      id_Delete(&out,b.owner);
      throw;
    }
    if(column!=l.rank) { id_Delete(&out,b.owner); throw std::runtime_error(
      "regular quotient formula rank mismatch"); }
    return out;
  }
  for(size_t j=0;j<l.rank;++j)
    for(size_t k=l.starts[j];k<l.starts[j+1];++k)
      out->m[j]=p_Add_q(out->m[j],entry(b,p,j,k,!firstIdeal),b.owner);
  return out;
}
static syStrategy explicitResolution(const PackedComplex& b) {
  const size_t maps=b.layers.size()-1;
  if (maps >= SHRT_MAX) throw std::runtime_error("resolution exceeds Singular length representation");
  syStrategy result=(syStrategy)omAlloc0(sizeof(ssyStrategy));
  result->length=std::max(size_t(1),maps)+1;
  result->list_length=result->length;
  result->minres=(resolvente)omAlloc0((result->length+1)*sizeof(ideal));
  if(maps==0) result->minres[0]=idInit(1,1);
  else for(size_t p=1;p<=maps;++p) result->minres[p-1]=expandLayer(b,p,p==1);
  result->minres[result->length-1]=idInit(1,maps?b.layers.back().rank:1);
  return result;
}
static BOOLEAN report(const std::exception& e) { Werror("monres: %s",e.what()); return TRUE; }
static BOOLEAN compute(leftv res,leftv args,bool sf,bool boxed) {
  try {
    Box b=build(args,sf);
    if(boxed) { res->data=new Box(std::move(b)); res->rtyp=boxType; }
    else { res->data=explicitResolution(*b); res->rtyp=RESOLUTION_CMD; }
    return FALSE;
  } catch(const std::exception& e) { return report(e); }
}
static BOOLEAN generalExplicit(leftv r,leftv a) { return compute(r,a,false,false); }
static BOOLEAN squarefreeExplicit(leftv r,leftv a) { return compute(r,a,true,false); }
static BOOLEAN generalBox(leftv r,leftv a) { return compute(r,a,false,true); }
static BOOLEAN squarefreeBox(leftv r,leftv a) { return compute(r,a,true,true); }
static BOOLEAN boxExplicit(leftv res,leftv a) {
  try { const auto& b=getBox(a); if(a->next) throw std::runtime_error("expected one monres object");
    res->data=explicitResolution(b); res->rtyp=RESOLUTION_CMD; return FALSE;
  } catch(const std::exception& e) { return report(e); }
}
static BOOLEAN boxRanks(leftv res,leftv a) {
  try { const auto& b=getBox(a); if(a->next) throw std::runtime_error("expected one monres object");
    intvec* ranks=new intvec(b.layers.size());
    for(size_t p=0;p<b.layers.size();++p) (*ranks)[p]=b.layers[p].rank;
    res->data=ranks; res->rtyp=INTVEC_CMD; return FALSE;
  } catch(const std::exception& e) { return report(e); }
}
static BOOLEAN boxDegrees(leftv res,leftv a) {
  try { const auto& b=getBox(a); const size_t p=getLevel(b,a->next,true);
    if(a->next->next) throw std::runtime_error("expected monres object and homological degree");
    if (b.owner->N!=0 && b.layers[p].rank > static_cast<size_t>(INT_MAX / b.owner->N))
      throw std::runtime_error("degree matrix dimensions exceed intmat capacity");
    intvec* d=new intvec(b.owner->N,b.layers[p].rank,0);
    if(b.regular && p!=0) {
      const RegularFormula& f=*b.regular;
      size_t column=0;
      try {
        regularCells(f,p,[&](size_t owner,const size_t* sigma,size_t sigmaSize) {
          for(int variable=0;variable<b.owner->N;++variable) {
            const long exponent=f.generatorExponent(owner,variable);
            if(exponent>INT_MAX) throw std::runtime_error(
              "multidegree exponent exceeds intmat capacity");
            (*d)[variable*b.layers[p].rank+column]=exponent;
          }
          for(size_t h=0;h<sigmaSize;++h) {
            const int variable=f.colonVariables[f.colonStarts[owner]+sigma[h]];
            const size_t index=variable*b.layers[p].rank+column;
            const int exponent=(*d)[index];
            if(exponent==INT_MAX) throw std::runtime_error(
              "multidegree exponent exceeds intmat capacity");
            (*d)[index]=exponent+1;
          }
          ++column;
        });
      } catch(...) {
        delete d;
        throw;
      }
      if(column!=b.layers[p].rank) { delete d; throw std::runtime_error(
        "regular quotient formula rank mismatch"); }
    } else {
      for(int v=0;v<b.owner->N;++v) for(size_t j=0;j<b.layers[p].rank;++j) {
        const long exponent=b.exponent(p,j,v);
        if(exponent>INT_MAX) { delete d; throw std::runtime_error(
          "multidegree exponent exceeds intmat capacity"); }
        (*d)[v*b.layers[p].rank+j]=exponent;
      }
    }
    res->data=d; res->rtyp=INTMAT_CMD; return FALSE;
  } catch(const std::exception& e) { return report(e); }
}
static BOOLEAN boxMatrix(leftv res,leftv a) {
  try { const auto& b=getBox(a); const size_t p=getLevel(b,a->next);
    if(a->next->next) throw std::runtime_error("expected monres object and homological degree");
    if(b.layers[p-1].rank && b.layers[p].rank > static_cast<size_t>(INT_MAX) / b.layers[p-1].rank)
      throw std::runtime_error("explicit differential dimensions exceed matrix capacity; use monresApply");
    res->data=id_Module2Matrix(expandLayer(b,p,false),b.owner); res->rtyp=MATRIX_CMD; return FALSE;
  } catch(const std::exception& e) { return report(e); }
}
static BOOLEAN boxApply(leftv res,leftv a) {
  try { const auto& b=getBox(a); const size_t p=getLevel(b,a->next);
    leftv v=a->next->next;
    if(!v || v->Typ()!=VECTOR_CMD || v->next)
      throw std::runtime_error("expected monres object, homological degree, and vector");
    const PackedLayer& l=b.layers[p]; poly input=(poly)v->Data();
    for(poly t=input;t;t=pNext(t))
      if(p_GetComp(t,b.owner)<1 || p_GetComp(t,b.owner)>l.rank)
        throw std::runtime_error("vector component exceeds the differential source rank");
    poly out=NULL;
    for(poly t=input;t;t=pNext(t)) {
      const size_t col=p_GetComp(t,b.owner)-1;
      if(b.regular) {
        regularBoundary(*b.regular,p,col,[&](const FormulaTerm& term) {
          poly m=p_Head(t,b.owner); p_SetComp(m,0,b.owner); p_Setm(m,b.owner);
          poly e=formulaEntry(b,term,true);
          if(!p_LmExpVectorAddIsOk(m,e,b.owner)) {
            p_Delete(&m,b.owner); p_Delete(&e,b.owner); p_Delete(&out,b.owner);
            throw std::runtime_error("monresApply exponent overflow; use a ring with a larger exponent bound");
          }
          out=p_Add_q(out,p_Mult_q(m,e,b.owner),b.owner);
        });
      } else for(size_t k=l.starts[col];k<l.starts[col+1];++k) {
        poly m=p_Head(t,b.owner); p_SetComp(m,0,b.owner); p_Setm(m,b.owner);
        poly e=entry(b,p,col,k,true);
        if(!p_LmExpVectorAddIsOk(m,e,b.owner)) {
          p_Delete(&m,b.owner); p_Delete(&e,b.owner); p_Delete(&out,b.owner);
          throw std::runtime_error("monresApply exponent overflow; use a ring with a larger exponent bound");
        }
        out=p_Add_q(out,p_Mult_q(m,e,b.owner),b.owner);
      }
    }
    res->data=out; res->rtyp=VECTOR_CMD; return FALSE;
  } catch(const std::exception& e) { return report(e); }
}
static void boxDestroy(blackbox*,void* d) { delete (Box*)d; }
static void* boxInit(blackbox*) { return new Box(); }
static void* boxCopy(blackbox*,void* d) { return d?new Box(*(Box*)d):new Box(); }
static char* boxString(blackbox*,void* d) {
  if(!d || !*(Box*)d) return omStrDup("uninitialized monres");
  const auto& b=**(Box*)d; std::ostringstream s;
  s<<"monres ("<<b.method<<"), ranks:";
  for(const auto& l:b.layers) s<<" "<<l.rank;
  s<<", "<<b.entries()<<" nonzero entries, "<<b.payloadBytes()<<" packed bytes";
  return omStrDup(s.str().c_str());
}
static BOOLEAN boxAssign(leftv l,leftv r) {
  if(r->Typ()!=boxType) { WerrorS("monres assignment requires a monres object"); return TRUE; }
  Box* copy=(Box*)boxCopy(NULL,r->Data()); delete (Box*)l->Data();
  if(l->rtyp==IDHDL) IDDATA((idhdl)l->data)=(char*)copy; else l->data=copy;
  return FALSE;
}
} // anonymous namespace
} // namespace monres

void iiInitMonres() {
  using namespace monres;
  if(boxType!=0) return;
  blackbox* bb=(blackbox*)omAlloc0(sizeof(blackbox));
  bb->blackbox_destroy=boxDestroy; bb->blackbox_Init=boxInit;
  bb->blackbox_Copy=boxCopy; bb->blackbox_Assign=boxAssign;
  bb->blackbox_String=boxString;
  boxType=setBlackboxStuff(bb,"monres");
  iiAddCproc("kernel","monomialResolution",FALSE,generalExplicit);
  iiAddCproc("kernel","squarefreeResolution",FALSE,squarefreeExplicit);
  iiAddCproc("kernel","monomialResolutionBox",FALSE,generalBox);
  iiAddCproc("kernel","squarefreeResolutionBox",FALSE,squarefreeBox);
  iiAddCproc("kernel","monresExplicit",FALSE,boxExplicit);
  iiAddCproc("kernel","monresRanks",FALSE,boxRanks);
  iiAddCproc("kernel","monresDegrees",FALSE,boxDegrees);
  iiAddCproc("kernel","monresMatrix",FALSE,boxMatrix);
  iiAddCproc("kernel","monresApply",FALSE,boxApply);
}

// Allows testing this exact translation unit against an installed libSingular;
// the normal build registers it directly through iiInitExtraCprocs().
#ifdef MONRES_STANDALONE_TEST
extern "C" int mod_init(SModulFunctions*) {
  package saved=currPack; currPack=basePack; iiInitMonres(); currPack=saved;
  return MAX_TOK;
}
#endif
