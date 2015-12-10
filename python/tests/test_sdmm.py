""" Tests SDMM by reproducing example_30dor.c """


def test_l1_sdmm():
    from numpy.linalg import norm
    from numpy.testing import assert_allclose
    from numpy import sqrt, nonzero
    from purify.tests.random import reset
    from purify import SDMM
    from purify.tests.image_fixtures import image_and_visibilities, dirty_measurements

    image, visibility = image_and_visibilities()

    reset()
    wavelets = ['DB%i' % i for i in range(1, 9)] + ['Dirac']
    sdmm = SDMM(image_size=image.shape, nblevels=4, wavelets=wavelets)
    visibility['y'] = dirty_measurements(image, visibility)

    sigma = norm(visibility['y0']) * 10.0**(-1.5) / sqrt(len(visibility))
    radius = sqrt(1e0 + 2.0 / sqrt(len(visibility))) * sigma               \
        * sqrt(image.size)
    actual = sdmm(visibility, radius=radius, max_iter=5)

    expected = [
        0.1234369061976812+0.0245938905918068j, 0.0044202706093182+0.0007616897390088j,
        0.0080917029388988-0.0008334981295526j, 0.0007340804988519-0.0088779299908127j,
        0.0044315005400009+0.0023154436270295j, 0.0213145075658684-0.0051175490926557j,
       -0.0014133100097788+0.0015866794480593j, -0.0008425944636673-0.0046840992897016j,
        0.0256120587274096+0.0050171971397882j, 0.0557321068517087+0.0196060954280739j,
        0.0007292263672404-0.0033286573644768j, 0.0250824527502236+0.0030024658569484j,
        0.0757660309982392+0.0245665741666273j, 0.1446862856428723-0.0013689915747699j,
        0.0018050634624183-0.0050644154829820j, 0.0412071809273152-0.0052970296627593j,
        0.4221055758003921-0.0202740371103322j, 0.0053651037406029+0.0006988031422697j,
        0.0029694357449050+0.0040594929238817j, -0.0032753764067566-0.0059422773632728j,
        0.0045820186844835-0.0003790018452575j, 0.0129397475579664-0.0019293098002858j
    ]
    assert_allclose(actual.flat[nonzero(actual.flat)][::3000], expected, rtol=1e-4, atol=1e-5)


def test_l1_rw_sdmm():
    from numpy.linalg import norm
    from numpy.testing import assert_allclose
    from numpy import sqrt, nonzero
    from purify.tests.random import reset
    from purify import SDMM
    from purify.tests.image_fixtures import image_and_visibilities, dirty_measurements

    image, visibility = image_and_visibilities()

    reset()
    wavelets = ['DB%i' % i for i in range(1, 9)] + ['Dirac']
    rwsdmm = SDMM(image_size=image.shape, nblevels=4, wavelets=wavelets,
                  reweighted=True)
    visibility['y'] = dirty_measurements(image, visibility)

    sigma = norm(visibility['y0']) * 10.0**(-1.5) / sqrt(len(visibility))
    radius = sqrt(1e0 + 2.0 / sqrt(len(visibility))) * sigma               \
        * sqrt(image.size)
    rwsdmm.rw.sigma = sigma * sqrt(len(visibility)) \
        / sqrt(len(rwsdmm) * image.size)
    rwsdmm.rw.max_iter = 3
    rwsdmm.relative_variation = 1e-3
    rwsdmm.rw.relative_variation = 1e-3
    actual = rwsdmm(visibility, radius=radius, max_iter=3)

    expected = [
        6.9644327315040430e-02+0.j,  1.8297498345146565e-03+0.j,
        1.5553174630838782e-02+0.j,  4.5729959161253022e-04+0.j,
        4.2550898781740114e-02+0.j,  1.3963721510188315e-02+0.j,
        2.0657375296235635e-04+0.j,  9.9735448519936723e-02+0.j,
        1.9054703317961155e-01+0.j,  6.0294412973877784e-03+0.j,
        2.2372316072343920e-01+0.j,  1.3624828609494955e-02+0.j,
        1.8201075920335860e-02+0.j,  1.1789390353637153e-02+0.j
    ]
    assert_allclose(actual.flat[nonzero(actual.flat)][::3000], expected, rtol=1e-4, atol=1e-5)


def test_tv_sdmm():
    from numpy.linalg import norm
    from numpy.testing import assert_allclose
    from numpy import sqrt, nonzero
    from purify.tests.random import reset
    from purify import SDMM
    from purify.tests.image_fixtures import image_and_visibilities, dirty_measurements

    image, visibility = image_and_visibilities()

    reset()
    sdmm = SDMM(image_size=image.shape)

    visibility['y'] = dirty_measurements(image, visibility)

    sigma = norm(visibility['y0']) * 10.0**(-1.5) / sqrt(len(visibility))
    radius = sqrt(1e0 + 2.0 / sqrt(len(visibility))) * sigma               \
        * sqrt(image.size)
    actual = sdmm(visibility, radius=radius, max_iter=6, tv_norm=True,
                  relative_variation=1e-3)

    expected = [
         1.3037156907427577e-01+0.0213418063839752j, 6.1453337292311193e-03+0.0005152966398734j,
         1.0871201831462105e-02+0.0014016951842977j, 1.3089608339981380e-03-0.0126181699553363j,
         6.1606270836815826e-03+0.0039324647607711j, 2.7903197178989585e-02-0.0047487332332415j,
        -1.5597684607971361e-03+0.0036838752540815j, -6.9986016065199546e-04-0.0070632518677344j,
         3.0145476734976005e-02+0.0065475661821378j, 5.8518664729494192e-02+0.0253242966590757j,
         3.3777246564958954e-04-0.0047601403838311j, 3.1391515099110770e-02+0.0071090147907392j,
         9.6117452483081317e-02+0.0359632245656850j, 1.7233607290980041e-01-0.0016256092189459j,
         3.8145036525780952e-03-0.0079904996214698j, 4.3897287654178836e-02-0.0082898545963509j,
         4.7595504641899766e-01-0.0250632310326073j, 9.0570582066842528e-03+0.0017408882418116j,
         4.2282901387241485e-03+0.0047909068900142j, -2.4323138204504322e-03-0.0098950117134673j,
         5.3903805852120699e-03-0.0032442252974253j, 1.5631187780493085e-02-0.0039307340048184j
    ]
    assert_allclose(actual.flat[nonzero(actual.flat)][::3000], expected, rtol=1e-4, atol=1e-5)


def test_tv_rw_sdmm():
    from numpy.linalg import norm
    from numpy.testing import assert_allclose
    from numpy import sqrt, nonzero
    from purify.tests.random import reset
    from purify import SDMM
    from purify.tests.image_fixtures import image_and_visibilities, dirty_measurements

    image, visibility = image_and_visibilities()

    reset()
    wavelets = ['DB%i' % i for i in range(1, 9)] + ['Dirac']
    rwsdmm = SDMM(image_size=image.shape, nblevels=4, wavelets=wavelets,
                  reweighted=True, tv_norm=True)
    visibility['y'] = dirty_measurements(image, visibility)

    sigma = norm(visibility['y0']) * 10.0**(-1.5) / sqrt(len(visibility))
    radius = sqrt(1e0 + 2.0 / sqrt(len(visibility))) * sigma               \
        * sqrt(image.size)
    rwsdmm.rw.sigma = sigma * sqrt(len(visibility)) \
        / sqrt(len(rwsdmm) * image.size)
    rwsdmm.rw.max_iter = 2
    rwsdmm.relative_variation = 1e-3
    rwsdmm.rw.relative_variation = 1e-3
    rwsdmm.rw.warm_start = False
    actual = rwsdmm(visibility, radius=radius, max_iter=3)

    expected = [
        0.0739041952261278+0.j, 0.0089584645327621+0.j, 0.0106078149851005+0.j,
        0.0053959386490470+0.j, 0.0007931569659929+0.j, 0.0238125540602920+0.j,
        0.0106111205982131+0.j, 0.0288039312686412+0.j, 0.0153865852978835+0.j,
        0.0020920012698815+0.j, 0.0165551298028742+0.j, 0.3290960018595836+0.j,
        0.7328531163765605+0.j, 0.1224706716302944+0.j, 0.2199407788749409+0.j,
        0.1253179094359953+0.j, 0.1248870238118705+0.j, 0.0024309110296712+0.j,
        0.0048528828162891+0.j, 0.0068033084179105+0.j
    ]
    assert_allclose(actual.flat[nonzero(actual.flat)][::2000], expected, rtol=1e-4, atol=1e-5)
