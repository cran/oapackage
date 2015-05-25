\docType{package}
\name{oapackage-package}
\alias{oapackage-package}
\alias{oapackage}
\title{Orthogonal Array Package}
\description{
This package creates 2-level D-optimal designs with a user specified optimization function.
}
\details{
The main function in the package is \code{\link{Doptimize}}. This function generates a single D-optimal design.
The optimization function is specified using three parameters
\eqn{\alpha_1, \alpha_2, \alpha_3}{\alpha_1, \alpha_2, \alpha_3}. The following
function is then optimized:

\deqn{\alpha_1 D + \alpha_2 D_s + \alpha_3 D_1}{\alpha_1 D + \alpha_2 D_s + \alpha_3 D_1}

Here \eqn{D} is the \eqn{D}-efficiency of the design.

The \eqn{D_s}{D_s}- and \eqn{D_1}{D_1}-efficiency are defined as follows.
For a design \eqn{D} the model matrix
\eqn{X}{X} can be split into \eqn{[I X_1 X_{2}]}{[I X_1 X_{2}]}
with \eqn{I} the intercept, \eqn{X_1}{X_1} the main effects and \eqn{X_2}{X_2} the second order effects.
We let \eqn{X_{02}=[I X_2]} and define
\eqn{D_1=(|X_1^T X_1|)^{1/(k+1)}}, 
\eqn{D_s=(|X^T X|/|X_{02}^T X_{02} |)^{1/k}} with \eqn{k} the number of factors of the design. The \eqn{D_s} and \eqn{D_1} efficiencies are described in detail in the paper Eendebak (2015).

}
\author{
P.T. Eendebak <pieter.eendebak@gmail.com>

Alan Vazquez
}
\references{
    Eendebak, P.T. and Schoen, E.D. (2015) 
    \emph{Two-level designs to estimate all main effects and
two-factor interactions},
    submitted to Technometrics
    
}
\keyword{ array }
\keyword{ design }
\keyword{ D-optimal }
\seealso{
The documententation for \code{\link{Doptimize}}. 
For example code see the file example_Doptimize.R or example_design_usage.R included in the tests of the package.   
}
\examples{
p = Doptimize(N=32, k=7, nrestarts=20, alpha1=1, alpha2=1, alpha3=0)
}