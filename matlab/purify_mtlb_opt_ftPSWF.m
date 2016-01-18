function ftpswf = purify_mtlb_opt_ftPSWF(x, alpha)
% The special PSWF that is said to be optimal within radio astronomy
% Details are explained in Optimal Gridding of Visibility Data in Radio
% Astronomy, F. R. Schwab 1983.

if (alpha == 1) %the value of support and alpha that is standard in radio astronomy software, e.g. AIPS (VLA) and MIRIAD (ATCA).
    %polynomrial coefficients for rational apporximation
    p1 = [8.203343E-2,-3.644705E-1, 6.278660E-1, -5.335581E-1, 2.312756E-1, 2*0.0];
    p2 = [4.028559E-3,-3.697768E-2, 1.021332E-1, -1.201436E-1, 6.412774E-2, 2*0.];
    q1 = [1., 8.212018E-1, 2.078043E-1];
    q2 = [1., 9.599102E-1, 2.918724E-1];
    
    eta = 2 * x;
    
    num = eta*0;
    den = eta*0 + 1;
    eta1 = 0;
    eta2 = 0.75;
    temp = (eta2 >= abs(eta)) & (abs(eta) >= eta1);
    num(temp) = p1(end);
    eta(temp) = eta(temp).^2 - eta2.^2;
    %Calculating numerator and denominator using Horners rule.
    for i = 1:(length(p1)-1)
        k = length(p1) - i;
        num(temp) = eta(temp).*num(temp) +p1(k);
    end
    den(temp) = q1(end);
    for i = 1:(length(q1)-1)
        k = length(q1) - i;
        den(temp) = eta(temp).*den(temp) + q1(k);
    end
    
    eta1 = 0.75;
    eta2 = 1;
    eta = 2 * x;
    temp = (eta2 >= abs(eta)) &  (abs(eta) > eta1);
    eta(temp) = eta(temp).^2 - eta2.^2;
    num(temp) = p2(end);
    %Calculating numerator and denominator using Horners rule.
    for i = 1:(length(p2)-1)
        k = length(p2) - i;
        num(temp) = eta(temp).*num(temp) +p2(k);
    end
    den(temp) = q2(end);
    for i = 1:(length(q2)-1)
        k = length(q2) - i;
        den(temp) = eta(temp).*den(temp) + q2(k);
    end
    ftpswf = num./den;
    
end
end


