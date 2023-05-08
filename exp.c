/*
 * File:	exp.c
 * Author:	Jim Diamond
 * Date:	2020-06-05
 * Version:	1.0
 *
 * Purpose:	A program to test the rand_exp() function, which
 *		generates an exponentially-distributed random variable 
 *		with parameter lambda.  Note the 1/lambda is the mean
 *		of the distribution.
 *
 * Compile:	Needs -lm
 */

#include    <inttypes.h>
#include    <math.h>
#include    <stdio.h>
#include    <stdlib.h>

void
usage(const char * progname)
{
    fprintf(stderr, "Usage; %s [lambda (double)] [repetitions (int)]\n",
	    progname);
}


/*
 * Name:	rand_exp()
 * Purpose:	Generate an exponentially-distributed value rounded
 *		to 6 decimal digits of precision.
 * Arguments:	lamdba, the exponential distribution's parameter.
 * Outputs:	Nothing.
 * Modifies:	The random number stream.
 * Returns:	The generated random number.
 * Assumptions:	lamdba > 0.
 * Bugs:	?!
 * Notes:	The log() function must be the natural log.
 */

double
rand_exp(double lambda)
{
    int64_t divisor = (int64_t)RAND_MAX + 1;
    double u_0_to_almost_1;
    double raw_value;

    u_0_to_almost_1 = (double)random() / divisor;

    raw_value = log(1 - u_0_to_almost_1) / -lambda;

    return round(raw_value * 1000000.) / 1000000.;
}


int
main(int argc, char * argv[])
{
    int reps;
    double lambda;
    int i, ret1, ret2;

    if (argc !=3)
    {
	usage(argv[0]);
	return EXIT_FAILURE;
    }

    ret1 = sscanf(argv[1], "%lf", &lambda);
    ret2 = sscanf(argv[2], "%d", &reps);
    if (ret1 != 1 || ret2 != 1)
    {
	usage(argv[0]);
	return EXIT_FAILURE;
    }
    
    for (i = 0; i < reps; i++)
	printf("%.12lf\n", rand_exp(lambda));

    return EXIT_SUCCESS;
}
