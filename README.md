# Time-fractional Black-Scholes model based option pricing

## Classical Black–Scholes model: 

The price of a European option $V(t, x)$ is the solution of the boundary-value problem:

$$
\frac{\partial V}{\partial t} + rS \frac{\partial V}{\partial S} + \frac{1}{2} \sigma^2 S^2 \frac{\partial^2 V}{\partial S^2} - rV = 0
$$

### Boundary conditions
  - Terminal condition: $V(T, S) = \max(S - K, 0)$
  - Boundary condition: $V(t, 0) = 0$

Let $f(S_T) = V(T, S)$ be the payoff of the option at maturity. Then the theoretical price of the option under the B-S model is given by:

$$
V_0 = e^{-rT}  \mathbb{E} \left[ f(S_T) \right],
$$

### Caputo fractional derivative: 

For order $\alpha \in (0,1)$ Caputo fractional derivative is defined as:

$$
\frac{d^{\alpha}f}{dx^{\alpha}} = \frac{1}{\Gamma(1-\alpha)} \int_{0}^{x} \frac{f^{\prime}(t)}{(x-t)^{\alpha}} dt
$$

## Time-fractional Black–Scholes equation:

The fractional analogue is the boundary-value problem: 

$$
\frac{\partial^\alpha V}{\partial t^\alpha} + rS \frac{\partial V}{\partial S} + \frac{1}{2} \sigma^2 S^2 \frac{\partial^2 V}{\partial S^2} - rV = 0
$$

### Boundary conditions
- Terminal condition: $V(T, S) = \max(S - K, 0)$
- Boundary condition:
    $V(t, 0) = 0, \, V(t,x) \sim S - Ke^{-r(T-t)} \ \text{as} \ S \to \infty$

  
### The Monte Carlo Solution:

Using the similar idea with B-S there is an approach to calculate the theoretical price of option under the TFBSM:

$$
V \approx \frac{1}{N} \sum_{i=1}^{N} \left[ e^{-r \cdot E_T^{(i)}} \cdot \max\left( S_T^{(i)} - K, 0 \right) \right]
$$


1.  For each simulation `i`, generate a random operational time $E_T^{(i)}$ (the inverse α-stable subordinator). This random time introduces the "memory" and fat tails of the fractional model.

2.  Use this random time to calculate the asset price at expiration `T` for that path:
    
$$
S_T^{(i)} = S_0 \cdot \exp\left( (r - \frac{1}{2}\sigma^2) \cdot E_T^{(i)} + \sigma \sqrt{E_T^{(i)}} \cdot Z^{(i)} \right)
$$

$\quad\quad$ where $Z^{(i)}$ is a random draw from a standard normal distribution.

3.  For that path, compute the option's appropriate value at expiration:
    
$$
\text{Payoff}^{(i)} = \max( S_T^{(i)} - K, 0 )
$$

4.  Discount this payoff back to present value using the random operational time:
    
$$
\text{Discounted Payoff}^{(i)} = e^{-r \cdot E_T^{(i)}} \cdot \text{Payoff}^{(i)}
$$

5.  The final option price **V** is the average of all these discounted payoffs across all `N` simulated paths.

## Authors

Andrei Gumirov (@gavt45), Mariia Tokareva (@tokmashadm)

