# fast-akima-interpolation
Implementation of Akima spline interpolation with c/c++ intrinsics

AFAIK it is about 25% faster than auto vectorized code.

## Make
```
make test
make perf
```

## Example usage
```
int count = 10; //count of elements to be computed
std::vector<double> x(count); //x-values of input
std::vector<double> y(count); //y (dependend) values of input

//compute coefficients
FastAkima fastAkimaImpl;
auto coefs = fastAkimaImpl.computeCoefficients(count, x, y);

//coefs is vector of coefficients for cubic interpolation function. Vector is dividen in 4 parts, first is first coefs, etc.


//itnterpolate: @see tests for examples
```

## Licence
WTFPL

This is a school project for KIV/PPR @ zcu.cz
