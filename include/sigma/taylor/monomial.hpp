#pragma once
#include <atomic>
#include <map>
#include <ostream>
#include <sstream>
#include <stdexcept>

/** @file monomial.hpp
 *  @brief Defines the Monomial class
 */

namespace sigma {

/** @brief Represents a sparse multi-index exponent.
 *
 * A multi-index @f$\beta = (\beta_1, \ldots, \beta_v)@f$ assigns a
 * non-negative integer exponent to each of @f$v@f$ expansion variables, and
 * a Monomial identifies the corresponding monomial
 * @f$\delta^{\beta} = \delta_1^{\beta_1} \delta_2^{\beta_2} \cdots
 * \delta_v^{\beta_v}@f$. This is the exponent type used by the polynomial
 * part of a Taylor model (see docs/taylor.md).
 *
 * Expansion variables are opaque, process-wide unique identifiers minted by
 * make_variable(), exactly as Affine mints its error term symbols. A
 * Monomial only ever stores entries for variables with a nonzero exponent;
 * a Monomial with no entries is the constant monomial (degree 0, i.e. the
 * multiplicative identity).
 *
 * N.b. Monomial is not templated on a floating-point type. Its state is
 * exponents (non-negative integers), not the coefficients or radii that
 * Affine and Taylor associate with them.
 */
class Monomial {
public:
    /// Type used for indexing, offsets, and total degree
    using size_type = std::size_t;

    /// Opaque type used to identify an expansion variable
    using variable_t = size_type;

    /// Type used to store the exponent of a single variable
    using exponent_t = size_type;

    /// Type used to map variables to their exponents
    using exponents_t = std::map<variable_t, exponent_t>;

    // --- Constructors and Assignment ----------------------------------------

    /** @brief Constructs the constant monomial.
     *
     *  The constant monomial has no variables and represents the
     *  multiplicative identity, i.e. @f$\delta^{\beta} = 1@f$.
     *
     *  @throw none No throw guarantee
     */
    Monomial() = default;

    /** @brief Constructs a single-variable monomial.
     *
     *  The resulting monomial represents @f$\delta_{v}^{e}@f$ for the
     *  variable @p var and exponent @p exp.
     *
     *  @param[in] var The variable raised to a power in the resulting
     *                 monomial.
     *  @param[in] exp The exponent of @p var, defaults to 1.
     *
     *  @note This constructor is envisioned as primarily being used to
     *        initialize the first monomial. That really shouldn't result in a
     *        zero exponent, so passing a zero is likely a programming error.
     *        The default constructor is available if it really is supposed to
     *        be the constant monomial.
     *
     *  @throw std::invalid_argument If @p exp is 0. A Monomial only stores
     *                               variables with nonzero exponent, so a
     *                               single-variable monomial with exponent
     *                               0 is not representable by this ctor;
     *                               use the default ctor instead. Strong
     *                               throw guarantee.
     */
    explicit Monomial(variable_t var, exponent_t exp = 1) {
        if(exp == 0) {
            throw std::invalid_argument(
              "Monomial: exponent must be nonzero. Use the default "
              "constructor for the constant monomial instead.");
        }
        m_exponents_[var] = exp;
    }

    /** @brief Constructs a monomial from a full map of exponents.
     *
     *  This constructor allows for the creation of a monomial by specifying
     *  the full state of the monomial (the exponent of each variable). Any
     *  entries in @p exponents whose exponent is 0 are dropped, so the
     *  invariant that only nonzero exponents are stored holds regardless of
     *  what is passed in.
     *
     *  @note This ctor is envisioned as being used to initialize a monomial
     *        after arithmetic operations. It is natural for such operations to
     *        produce a map with zero entries, so this ctor will drop the zero
     *        terms instead of throwing.
     *
     *  @param[in] exponents A map of variables to their exponents.
     *
     *  @throw std::bad_alloc If memory allocation for the exponent map
     *                        fails. Strong throw guarantee.
     */
    explicit Monomial(exponents_t exponents) {
        for(auto&& [var, exp] : exponents) {
            if(exp != 0) { m_exponents_[var] = exp; }
        }
    }

    /// Makes a deep copy of @p other.
    Monomial(const Monomial& other) = default;

    /// Constructs a monomial by moving @p other.
    Monomial(Monomial&& other) noexcept = default;

    /// Assigns the value of @p other to this monomial.
    Monomial& operator=(const Monomial& other) = default;

    /// Moves the value of @p other to this monomial.
    Monomial& operator=(Monomial&& other) noexcept = default;

    // -- State Accessors -----------------------------------------------------

    /** @brief Returns the exponents of the monomial.
     *
     *  @return A map from variable to its (nonzero) exponent.
     *
     *  @throw none No throw guarantee
     */
    const exponents_t& exponents() const noexcept { return m_exponents_; }

    /** @brief Returns the exponent of a single variable.
     *
     *  @param[in] var The variable whose exponent is requested.
     *
     *  @return The exponent of @p var, or 0 if @p var does not appear in
     *          *this.
     *
     *  @throw none No throw guarantee
     */
    exponent_t exponent(variable_t var) const {
        auto it = m_exponents_.find(var);
        return it == m_exponents_.end() ? exponent_t{0} : it->second;
    }

    /** @brief Returns the total degree of the monomial.
     *
     *  The total degree is @f$|\beta| = \beta_1 + \beta_2 + \cdots +
     *  \beta_v@f$, the sum of the exponents over every variable.
     *
     *  @return The total degree of *this.
     *
     *  @throw none No throw guarantee
     */
    size_type degree() const noexcept {
        size_type d = 0;
        for(auto&& [var, exp] : m_exponents_) { d += exp; }
        return d;
    }

    /** @brief Checks if *this is the constant monomial.
     *
     *  N.b. unlike Affine::empty(), which denotes the empty set, this
     *  denotes the constant monomial (degree 0, no variables) -- a
     *  perfectly ordinary, non-empty-set value.
     *
     *  @return True if *this has no variables (equivalently, degree() ==
     *          0), false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool empty() const noexcept { return m_exponents_.empty(); }

    // -- Arithmetic Operators ------------------------------------------------

    /** @brief Overwrites *this with the product of *this and @p other.
     *
     *  The product of two monomials is the monomial whose exponent for each
     *  variable is the sum of the two operands' exponents for that
     *  variable:
     *  @f[
     *    \delta^{\beta_1} \cdot \delta^{\beta_2} = \delta^{\beta_1 +
     *    \beta_2}
     *  @f]
     *  A variable that appears in only one operand is carried over to the
     *  result unchanged, as if it had exponent 0 in the operand that lacks
     *  it.
     *
     *  @param[in] other The monomial to multiply *this by.
     *
     *  @return Reference to this monomial after multiplication.
     *
     *  @throw std::bad_alloc If memory allocation for the new exponent map
     *                        fails. Strong throw guarantee.
     */
    Monomial& operator*=(const Monomial& other) {
        for(auto&& [var, exp] : other.m_exponents_) {
            m_exponents_[var] += exp;
        }
        return *this;
    }

    /** @brief Returns the product of *this and @p other.
     *
     *  This is a convenience method for calling `Monomial(*this) *= other`.
     *  See the documentation for operator*=(Monomial) for details on how
     *  multiplication works.
     *
     *  @param[in] other The monomial to multiply *this by.
     *
     *  @return The product of *this and @p other.
     *
     *  @throw std::bad_alloc If memory allocation for the new exponent map
     *                        fails. Strong throw guarantee.
     */
    Monomial operator*(const Monomial& other) const {
        return Monomial(*this) *= other;
    }

    // -- Comparison Operators ------------------------------------------------

    /** @brief Checks if *this and @p other represent the same monomial.
     *
     *  Two monomials are equal if they have the same exponent for every
     *  variable (equivalently, if their exponents() maps compare equal).
     *
     *  @param[in] other The monomial to compare with *this.
     *
     *  @return True if *this and @p other represent the same monomial,
     *          false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool operator==(const Monomial& other) const {
        return m_exponents_ == other.m_exponents_;
    }

    /** @brief Checks if *this and @p other represent different monomials.
     *
     *  @param[in] other The monomial to compare with *this.
     *
     *  @return True if *this and @p other represent different monomials,
     *          false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool operator!=(const Monomial& other) const { return !(*this == other); }

    /** @brief Defines a strict weak ordering over monomials.
     *
     *  This ordering has no particular mathematical significance; it exists
     *  so that Monomial can be used as the key type of an ordered
     *  associative container (e.g. the coefficient map of a Taylor
     *  polynomial). It is implemented as the lexicographic order induced by
     *  std::map::operator< on exponents().
     *
     *  @param[in] other The monomial to compare with *this.
     *
     *  @return True if *this orders before @p other, false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool operator<(const Monomial& other) const {
        return m_exponents_ < other.m_exponents_;
    }

    /** @brief Creates a string representation of *this.
     *
     *  The constant monomial is printed as "1"; otherwise each variable
     *  with a nonzero exponent is printed as "v<id>^<exponent>", joined by
     *  " * ", in variable order.
     *
     *  @return A string representation of *this.
     *
     *  @throw std::bad_alloc If memory allocation for the string fails.
     *                        Strong throw guarantee.
     */
    std::string print() const {
        if(empty()) { return "1"; }
        std::stringstream ss;
        bool first = true;
        for(auto&& [var, exp] : m_exponents_) {
            if(!first) { ss << " * "; }
            ss << "v" << var << "^" << exp;
            first = false;
        }
        return ss.str();
    }

    /// Returns a process-wide unique integer ID for a new expansion
    /// variable.
    static variable_t make_variable() {
        static std::atomic<size_type> s_next_id{0};
        return s_next_id.fetch_add(1);
    }

private:
    /// Map from variable to its (nonzero) exponent. Empty when *this is the
    /// constant monomial.
    exponents_t m_exponents_;
};

// -- Non-member functions -----------------------------------------------

/** @brief Outputs a monomial to an output stream.
 *
 *  @related Monomial
 *
 *  This method defers to Monomial::print() to build the string that is
 *  written to @p os.
 *
 *  @param[in] os The output stream to write to.
 *  @param[in] m The monomial to output.
 *
 *  @return The output stream.
 *
 *  @throw none No throw guarantee.
 */
inline std::ostream& operator<<(std::ostream& os, const Monomial& m) {
    os << m.print();
    return os;
}

} // namespace sigma

namespace std {

/// Specializes std::hash so that Monomial can be used as the key type of an
/// unordered associative container.
template<>
struct hash<sigma::Monomial> {
    std::size_t operator()(const sigma::Monomial& m) const noexcept {
        std::size_t seed = 0;
        for(auto&& [var, exp] : m.exponents()) {
            auto combine = [&seed](std::size_t value) {
                // Same mixing constant/shifts as boost::hash_combine.
                seed ^= std::hash<std::size_t>{}(value) + 0x9e3779b9 +
                        (seed << 6) + (seed >> 2);
            };
            combine(var);
            combine(exp);
        }
        return seed;
    }
};

} // namespace std
