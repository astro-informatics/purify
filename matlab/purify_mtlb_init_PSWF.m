function [pswf,V, n] = purify_mtlb_init_PSWF(tau, samples, sigma)
% Calculation of the first prolate spherodial wave function, using method by Walter and Soleski 2005.
% :param: tau: The support/energy concentration interval in position domain
% (this calculation method is limited to tau<7).
% :param: samples: number of integer samples used to construct PSWF
% :param: sigma: how bandlimited the PSWF is in the Fourier Domain.

    fun = @(t, n, k, s) sinc((t-n)*s/pi).*sinc((t-k)*s/pi);
    A = @(n, k, s) integral(@(t)fun(t, n, k, s),-tau,tau);

    N = samples/2;

    At = zeros(2*N+1,2*N+1);

    for i = (-N):N
        for j = (-N):N
            At(N+i+1, N+j+1) = A(i,j, sigma);
        end
    end

    [V, ~] = eigs(At,1);
    [~,I] = max(abs(V));
    V = V*sign(V(I));
    n = ((-N):N);
    n = n';
    pswf = @(x) 0;
    for i = 1:(2*N+1)
        pswf = @(x) pswf(x)+V(i).*sinc((x-n(i))*sigma/pi);
    end
    lambda  = integral(@(x) pswf(x).^2, -tau, tau);
    pswf = @(x) pswf(x)/lambda;

end