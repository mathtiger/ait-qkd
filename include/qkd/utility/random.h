/*
 * random.h
 * 
 * random number generator interface
 *
 * Author: Oliver Maurhart, <oliver.maurhart@ait.ac.at>
 *
 * Copyright (C) 2012-2016 AIT Austrian Institute of Technology
 * AIT Austrian Institute of Technology GmbH
 * Donau-City-Strasse 1 | 1220 Vienna | Austria
 * http://www.ait.ac.at
 *
 * This file is part of the AIT QKD Software Suite.
 *
 * The AIT QKD Software Suite is free software: you can redistribute 
 * it and/or modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation, either version 3 of 
 * the License, or (at your option) any later version.
 * 
 * The AIT QKD Software Suite is distributed in the hope that it will 
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with the AIT QKD Software Suite. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

 
#ifndef __QKD_UTILITY_RANDOM_H_
#define __QKD_UTILITY_RANDOM_H_


// ------------------------------------------------------------
// incs

#include <exception>
#include <memory>
#include <string>

#include <inttypes.h>

// ait
#include <qkd/exception/randomengine_error.h>
#include <qkd/utility/memory.h>


// ------------------------------------------------------------
// defs

#if defined(__GNUC__) || defined(__GNUCPP__)
#   define UNUSED   __attribute__((unused))
#else
#   define UNUSED
#endif


// ------------------------------------------------------------
// decls


namespace qkd {
    
namespace utility {    


// fwd
class random_source;
typedef std::shared_ptr<random_source> random;


/**
 * this encapsulates a random number source
 * 
 * a random source is created by calling
 * 
 *      create(URL);
 * 
 * with a proper URL.
 * 
 * If the URL is empty, than a C API random source ("c-api")
 * using rand() is created.
 * 
 * An URL might look like this:
 * 
 *      file:///dev/urandom
 * 
 * Whenever the end of a file has been reached (eof)
 * the random source starts reading anew from the
 * beginning of the file.
 * 
 * Current supported URLs are:
 * 
 *      file://filepath                 read random values from a file
 *      c-api:[seed]                    use POSIX C api rand(), optionally with a seed value
 *      hmac-sha:key                    use hmac with SHA, depending on key size with 256, 384 or 512 bits
 *      cbs-aes:key                     use cypher-block-chaining AES, depending on key size with 128, 192 or 256 bits
 *      linear-congruential:[seed]      use linear-congruential algorithm of C++11 with an optional seed
 * 
 * Hence, the floating point retrievals (float and double) do
 * return values in the range [0.0, 1.0)
 */
class random_source {

    
    friend class random_singelton;

    
public:
    
    
    /**
     * This type definition exists to satisfy the UniformRandomNumberGenerator concept.
     */
    typedef int64_t result_type;

    
    /**
     * This class method exists to satisfy the UniformRandomNumberGenerator concept and returns
     * the lower bound of the domain used by this number generator.
     *
     * @return  returns the smallest value that the operator() may return.
     */
    static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }

    
    /**
     * This class method exists to satisfy the UniformRandomNumberGenerator concept and returns
     * the upper bound of the domain used by this number generator.
     *
     * @return  returns the largest value that the operator() may return.
     */
    static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

    
    /**
     * This method exists to satisfy the UniformRandomNumberGenerator concept and returns
     * a value indicating the entropy of this random number generator, depending on the
     * underlying implementation and defaulting to 0 (the worst case). A deterministic random
     * number generator (e.g. a pseudo-random engine) has entropy zero.
     *
     * @return   value indicating the entropy of this random number generator where 0 is a
     * deterministic random number generator.
     */
    virtual double entropy() const { return 0.; }
    

    /**
     * This method exists to satisfy the UniformRandomNumberGenerator concept and returns
     * a value in the closed interval.
     *
     * @return  a number between min() and max()
     */
    result_type operator()() {
        result_type i;
        get((char*)&i, sizeof(i));
        return i;
    }


    /**
     * dtor
     */
    virtual ~random_source() {};


    /**
     * stream out
     * 
     * Get the next random value.
     *
     * @param   rhs         right hand side: the next random value
     * @return  the random object
     */
    inline random_source & operator>>(char & rhs) { 
        get((char*)&rhs, sizeof(rhs)); 
        return *this; 
    }


    /**
     * stream out
     * 
     * Get the next random value.
     *
     * @param   rhs         right hand side: the next random value
     * @return  the random object
     */
    inline random_source & operator>>(unsigned char & rhs) { 
        get((char*)&rhs, sizeof(rhs)); 
        return *this; 
    }


    /**
     * stream out
     *
     * Get the next random value.
     *
     * @param   rhs         right hand side: the next random value
     * @return  the random object
     */
    inline random_source & operator>>(int32_t & rhs) { 
        get((char*)&rhs, sizeof(rhs)); 
        return *this; 
    }


    /**
     * stream out
     *
     * Get the next random value.
     *
     * @param   rhs         right hand side: the next random value
     * @return  the random object
     */
    inline random_source & operator>>(uint32_t & rhs) { 
        get((char*)&rhs, sizeof(rhs)); 
        return *this; 
    }


    /**
     * stream out
     *
     * Get the next random value.
     *
     * @param   rhs         right hand side: the next random value
     * @return  the random object
     */
    inline random_source & operator>>(int64_t & rhs) { 
        get((char*)&rhs, sizeof(rhs)); 
        return *this; 
    }


    /**
     * stream out
     *
     * Get the next random value.
     *
     * @param   rhs         right hand side: the next random value
     * @return  the random object
     */
    inline random_source & operator>>(uint64_t & rhs) { 
        get((char*)&rhs, sizeof(rhs)); 
        return *this; 
    }


    /**
     * stream out
     *
     * Get the next random value in [0.0, 1.0).
     *
     * @param   rhs         right hand side: the next random value
     * @return  the random object
     */
    inline random_source & operator>>(double & rhs) { 
        uint64_t a; 
        uint64_t b; 
        (*this) >> a; 
        (*this) >> b; 
        if (a > b) rhs = (double)b / (double)a; 
        if (b > a) rhs = (double)a / (double)b; 
        if (a == b) rhs = 0.0; 
        return *this; 
    }


    /**
     * stream out
     *
     * Get the next random value in [0.0, 1.0).
     *
     * @param   rhs         right hand side: the next random value
     * @return  the random object
     */
    inline random_source & operator>>(float & rhs) { 
        uint64_t a; 
        uint64_t b; 
        (*this) >> a; 
        (*this) >> b; 
        if (a > b) rhs = (float)b / (float)a; 
        if (b > a) rhs = (float)a / (float)b; 
        if (a == b) rhs = 0.0; 
        return *this; 
    }


    /**
     * stream out
     *
     * Get the next random value.
     *
     * @param   rhs         right hand side: the next random value
     * @return  the random object
     */
    inline random_source & operator>>(qkd::utility::memory & rhs) { 
        get((char *)rhs.get(), rhs.size()); 
        return *this; 
    }


    /**
     * factory method to create a random source
     * 
     * if the url is left empty, the C-API rand() and srand() is used as source
     *
     * @param   sURL        a URL string indicating the random source
     * @return  an initialized random object
     */
    static random create(std::string sURL = "");
    
    
    /**
     * describe the random source
     * 
     * @return  a HR-string describing the random source
     */
    virtual std::string describe() const { return "NULL random"; }


    /**
     * sets the main random singleton source
     *
     * @param   cRandom     the new random singleton source
     */
    static void set_source(random & cRandom);


    /**
     * returns the main random singleton source
     *
     * @return  the main random singleton
     */
    static random & source();


    /**
     * ctor
     */
    random_source() {};

    /**
     * iff the underlying implementation supports it, the random source will be seeded
     * with the specified value.
     *
     * @param   seed    the seed to use, if applicable.
     */
    virtual void seed(UNUSED result_type seed) { };
    
    
private:

    
    /**
     * get a block of random bytes
     * 
     * This function must be overwritten in derived classes
     * 
     * @param   cBuffer     buffer which will hold the bytes
     * @param   nSize       size of buffer in bytes
     */
    virtual void get(UNUSED char * cBuffer, UNUSED uint64_t nSize) { 
        throw qkd::exception::randomengine_error("failed to provide implementation for random data retrieval");
    };
    
};


/**
 * stream out
 * 
 * Get the next random value
 * 
 * @param   lhs     the random object
 * @param   rhs     the next random value
 * @return  the random object
 */
inline random & operator>>(random & lhs, char & rhs) { 
    (*lhs) >> rhs; 
    return lhs; 
}


/**
 * stream out
 * 
 * Get the next random value
 * 
 * @param   lhs     the random object
 * @param   rhs     the next random value
 * @return  the random object
 */
inline random & operator>>(random & lhs, unsigned char & rhs) { 
    (*lhs) >> rhs; 
    return lhs; 
}


/**
 * stream out
 * 
 * Get the next random value
 * 
 * @param   lhs     the random object
 * @param   rhs     the next random value
 * @return  the random object
 */
inline random & operator>>(random & lhs, int32_t & rhs) { 
    (*lhs) >> rhs; 
    return lhs; 
}


/**
 * stream out
 * 
 * Get the next random value
 * 
 * @param   lhs     the random object
 * @param   rhs     the next random value
 * @return  the random object
 */
inline random & operator>>(random & lhs, uint32_t & rhs) { 
    (*lhs) >> rhs; 
    return lhs; 
}


/**
 * stream out
 * 
 * Get the next random value
 * 
 * @param   lhs     the random object
 * @param   rhs     the next random value
 * @return  the random object
 */
inline random & operator>>(random & lhs, int64_t & rhs) { 
    (*lhs) >> rhs; 
    return lhs; 
}

    
/**
 * stream out
 * 
 * Get the next random value
 * 
 * @param   lhs     the random object
 * @param   rhs     the next random value
 * @return  the random object
 */
inline random & operator>>(random & lhs, uint64_t & rhs) { 
    (*lhs) >> rhs; 
    return lhs; 
}


/**
 * stream out
 * 
 * Get the next random value
 * 
 * @param   lhs     the random object
 * @param   rhs     the next random value
 * @return  the random object
 */
inline random & operator>>(random & lhs, float & rhs) { 
    (*lhs) >> rhs; 
    return lhs; 
}


/**
 * stream out
 * 
 * Get the next random value
 * 
 * @param   lhs     the random object
 * @param   rhs     the next random value
 * @return  the random object
 */
inline random & operator>>(random & lhs, double & rhs) { 
    (*lhs) >> rhs; 
    return lhs; 
}


/**
 * stream out
 * 
 * Get the next random value
 * 
 * @param   lhs     the random object
 * @param   rhs     the next random value
 * @return  the random object
 */
inline random & operator>>(random & lhs, qkd::utility::memory & rhs) { 
    (*lhs) >> rhs; 
    return lhs; 
}
    

    
}

}

#endif

