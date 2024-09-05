# Background

Given independent variables \f$ a = \bar{a} \pm \sigma_{a} \f$ and 
\f$ b = \bar{b} \pm \sigma_{b} \f$, where \f$\bar{a}\f$ is the median value
of the first variable and \f$\sigma_{a}\f$ is the standard deviation of that 
variable, the error of a value \f$c\f$ derived from operations on \f$a\f$ and 
\f$b\f$ can be determined as

\f[ \large
  \sigma_{c} = \sqrt{(\frac{\delta c}{\delta a}|_{a=\bar{a}}\sigma_{a})^2 + 
  (\frac{\delta c}{\delta b}|_{b=\bar{b}}\sigma_{b})^2}
\f]