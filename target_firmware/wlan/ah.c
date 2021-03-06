#include "opt_ah.h"
#include "ah.h"
#include "ah_internal.h"
#include <asf_bitmap.h>

extern struct ath_hal *ar5416Attach(a_uint32_t devid,HAL_SOFTC sc, adf_os_device_t dev,
				    HAL_BUS_HANDLE sh, a_uint32_t flags, HAL_STATUS *status);

struct ath_hal*
ath_hal_attach_tgt(a_uint32_t devid,HAL_SOFTC sc,
		   adf_os_device_t dev, HAL_BUS_HANDLE sh,
		   a_uint32_t flags, HAL_STATUS *error)
{
	struct ath_hal *ah = AH_NULL;

	devid = AR5416_DEVID_PCIE;
	ah = ar5416Attach(devid, sc, dev, sh, flags, error);

	return ah;
}

HAL_STATUS
ath_hal_getcapability(struct ath_hal *ah, HAL_CAPABILITY_TYPE type,
		      a_uint32_t capability, a_uint32_t *result)

{
	const HAL_CAPABILITIES *pCap = &AH_PRIVATE(ah)->ah_caps;
	switch (type) {
	case HAL_CAP_TSF_ADJUST:
		return HAL_ENOTSUPP;
	case HAL_CAP_BSSIDMASK:
		return pCap->halBssIdMaskSupport ? HAL_OK : HAL_ENOTSUPP;
	case HAL_CAP_VEOL:
		return pCap->halVEOLSupport ? HAL_OK : HAL_ENOTSUPP;
#ifdef MAGPIE_MERLIN
	case HAL_CAP_RX_STBC:
		return HAL_ENOTSUPP;
	case HAL_CAP_TX_STBC:
		return HAL_ENOTSUPP;
#endif
	default:
		return HAL_EINVAL;
	}
}

void
ath_hal_setupratetable(struct ath_hal *ah, HAL_RATE_TABLE *rt)
{
	a_int32_t i;

	if (rt->rateCodeToIndex[0] != 0)
		return;

	for (i = 0; i < 32; i++)
		rt->rateCodeToIndex[i] = (a_uint8_t) -1;
	for (i = 0; i < rt->rateCount; i++) {
		a_uint8_t code = rt->info[i].rateCode;
		a_uint8_t cix = rt->info[i].controlRate;

		rt->rateCodeToIndex[code] = i;
		rt->rateCodeToIndex[code | rt->info[i].shortPreamble] = i;
		rt->info[i].lpAckDuration = ath_hal_computetxtime(ah, rt,
					  WLAN_CTRL_FRAME_SIZE, cix, AH_FALSE);
		rt->info[i].spAckDuration = ath_hal_computetxtime(ah, rt,
					  WLAN_CTRL_FRAME_SIZE, cix, AH_TRUE);
	}
}

#define CCK_SIFS_TIME        10
#define CCK_PREAMBLE_BITS   144
#define CCK_PLCP_BITS        48

#define OFDM_SIFS_TIME        16
#define OFDM_PREAMBLE_TIME    20
#define OFDM_PLCP_BITS        22
#define OFDM_SYMBOL_TIME       4

#define OFDM_SIFS_TIME_HALF     32
#define OFDM_PREAMBLE_TIME_HALF 40
#define OFDM_PLCP_BITS_HALF     22
#define OFDM_SYMBOL_TIME_HALF   8

#define OFDM_SIFS_TIME_QUARTER      64
#define OFDM_PREAMBLE_TIME_QUARTER  80
#define OFDM_PLCP_BITS_QUARTER      22
#define OFDM_SYMBOL_TIME_QUARTER    16

a_uint16_t
ath_hal_computetxtime(struct ath_hal *ah,
		      const HAL_RATE_TABLE *rates, a_uint32_t frameLen, a_uint16_t rateix,
		      HAL_BOOL shortPreamble)
{
	a_uint32_t bitsPerSymbol, numBits, numSymbols, phyTime, txTime;
	a_uint32_t kbps;

	kbps = rates->info[rateix].rateKbps;

	/*
	 * index can be invalid duting dynamic Turbo transitions.
	 */
	if(kbps == 0) return 0;
	switch (rates->info[rateix].phy) {

	case IEEE80211_T_CCK:
		phyTime = CCK_PREAMBLE_BITS + CCK_PLCP_BITS;
		if (shortPreamble && rates->info[rateix].shortPreamble)
			phyTime >>= 1;
		numBits = frameLen << 3;
		txTime = phyTime + ((numBits * 1000)/kbps);
		/* TODO: make sure the same value of txTime can use in all device */
		if (ath_hal_getcapability(ah, HAL_CAP_HT, 0, AH_NULL) != HAL_OK)
			txTime = txTime + CCK_SIFS_TIME;
		break;
	case IEEE80211_T_OFDM:
		/* full rate channel */
		bitsPerSymbol   = (kbps * OFDM_SYMBOL_TIME) / 1000;
		HALASSERT(bitsPerSymbol != 0);

		numBits = OFDM_PLCP_BITS + (frameLen << 3);
		numSymbols = asf_howmany(numBits, bitsPerSymbol);
		txTime = OFDM_PREAMBLE_TIME + (numSymbols * OFDM_SYMBOL_TIME);
		/* TODO: make sure the same value of txTime can use in all device */
		if (ath_hal_getcapability(ah, HAL_CAP_HT, 0, AH_NULL) != HAL_OK)
			txTime = txTime + OFDM_SIFS_TIME;
		break;
	default:
		txTime = 0;
		break;
	}
	return txTime;
}

#undef CCK_SIFS_TIME
#undef CCK_PREAMBLE_BITS
#undef CCK_PLCP_BITS

#undef OFDM_SIFS_TIME
#undef OFDM_PREAMBLE_TIME
#undef OFDM_PLCP_BITS
#undef OFDM_SYMBOL_TIME

#ifdef MAGPIE_MERLIN
a_uint32_t 
ath_hal_get_curmode(struct ath_hal *ah, HAL_CHANNEL_INTERNAL *chan)
{
	if (!chan)
		return HAL_MODE_11NG;

	if (IS_CHAN_NA(chan))
		return HAL_MODE_11NA; 

	if (IS_CHAN_A(chan))
		return HAL_MODE_11A;

	if (IS_CHAN_NG(chan))
		return HAL_MODE_11NG;

	if (IS_CHAN_G(chan))
		return HAL_MODE_11G;

	if (IS_CHAN_B(chan))
		return HAL_MODE_11B;

	HALASSERT(0);
	return HAL_MODE_11NG;
}

#endif

HAL_BOOL
ath_hal_wait(struct ath_hal *ah, a_uint32_t reg, a_uint32_t mask, a_uint32_t val)
{
#define AH_TIMEOUT_11N 100000
#define AH_TIMEOUT_11G  1000

	a_int32_t i;

	if (ath_hal_getcapability(ah, HAL_CAP_HT, 0, AH_NULL) == HAL_OK) {
		for (i = 0; i < AH_TIMEOUT_11N; i++) {
			if ((OS_REG_READ(ah, reg) & mask) == val)
				return AH_TRUE;
			OS_DELAY(10);
		}
	} else {
		for (i = 0; i < AH_TIMEOUT_11G; i++) {
			if ((OS_REG_READ(ah, reg) & mask) == val)
				return AH_TRUE;
			OS_DELAY(10);
		}
	}
	return AH_FALSE;

#undef AH_TIMEOUT_11N
#undef AH_TIMEOUT_11G
}
