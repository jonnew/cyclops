
% Feedback factor, Rsense = 0.5 Ohms
v_out = [5.5:0.5:12.5];
v_invert = [0, 2.946801e-006, 3.097973e-006, 1.977853e-003, ...
    3.793564e-002, 1.215662e-001, 2.304090e-001, 3.530959e-001, ...
    4.845515e-001, 6.221187e-001, 7.642141e-001, 9.097936e-001, ...
    1.058085e+000, 1.208356e+000, 1.340258e+000 ];

close all
figure
[hAx,hLine1,hLine2] = plotyy(v_out, v_invert, v_out, v_invert./v_out)
xlabel('v_{out} (v)')

ylabel(hAx(1),'v_{invert} (v)') % left y-axis
ylabel(hAx(2),'F') % right y-axis
