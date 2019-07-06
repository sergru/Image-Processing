#include "stdafx.h"

#include "CppUnitTest.h"
#include "IjsFilterAdditiveNoiseRemoval.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(UT_CIjsFilterAdditiveNoiseRemoval)
	{
  private:
    CString strMessage;

	public:

    // ================================================================================================================
    TEST_METHOD(Test_CIjsFilterAdditiveNoiseRemoval)
    {
      CIjsFilterAdditiveNoiseRemoval ijsANR;
      Assert::AreEqual<BYTE>(BASE_NOISE, ijsANR.UT_GetBaseNoise(), _T("Constructor failed."));
    }

    // ================================================================================================================
    TEST_METHOD(Test_SetBaseNoise)
    {
      CIjsFilterAdditiveNoiseRemoval ijsANR;
      const BYTE bTestVal = 103;
      ijsANR.SetBaseNoise(bTestVal);

      Assert::AreEqual<BYTE>(bTestVal, ijsANR.UT_GetBaseNoise(), _T("SetBaseNoise faled."));
    }

    // ================================================================================================================
    TEST_METHOD(Test_GetIterationLimits)
		{
      CIjsFilterAdditiveNoiseRemoval ijsANR;

      ULONG firstX;
      ULONG lastX;
      ULONG firstY;
      ULONG lastY;

      ijsANR.GetIterationLimits(firstX, lastX, firstY, lastY);

      Assert::AreEqual<ULONG>(-1, firstX, _T("Incorrect firstX"));
      Assert::AreEqual<ULONG>(-1, lastX, _T("Incorrect lastX"));
      Assert::AreEqual<ULONG>(-1, firstY, _T("Incorrect firstY"));
      Assert::AreEqual<ULONG>(-1, lastY, _T("Incorrect lastY"));
    }

    // ================================================================================================================
    TEST_METHOD(Test_GetCalibrationValue)
    {
      CIjsFilterAdditiveNoiseRemoval ijsANR;

      ULONG pixel = 10;
      Assert::AreEqual<BYTE>(BASE_NOISE, ijsANR.UT_GetCalibrationValue(pixel, pixel),
        _T("It should be base noise without calibration image."));
    }

    // ================================================================================================================
    TEST_METHOD(Test_SetCalibrationFile)
    {
      LPCTSTR pszUnacceptabeFile =  _T("../../dist/images/AdvancedTechnicalAssessment.pdf");
      LPCTSTR pszUnsupportedFile = _T("../../images/whale.jpg");
      LPCTSTR pszSupportedFile = _T("../../images/whale_blur.png");
      HRESULT hr;

      CIjsFilterAdditiveNoiseRemoval ijsANR;

      hr = ijsANR.SetCalibrationFile(NULL);
      Assert::AreEqual<HRESULT>(E_INVALIDARG, hr, _T("SetCalibrationFile failed to return correct error code."));

      hr = ijsANR.SetCalibrationFile(_T(""));
      Assert::AreEqual<HRESULT>(E_INVALIDARG, hr, _T("SetCalibrationFile failed to return correct error code."));

      hr = ijsANR.SetCalibrationFile(_T("NO_THIS_FILE.png"));
      Assert::AreEqual<HRESULT>(E_ACCESSDENIED, hr, _T("SetCalibrationFile failed to return correct error code."));

      hr = ijsANR.SetCalibrationFile(pszUnacceptabeFile);
      Assert::AreEqual<HRESULT>(E_ACCESSDENIED, hr, _T("SetCalibrationFile failed to return correct error code."));

      hr = ijsANR.SetCalibrationFile(pszUnsupportedFile);
      Assert::AreEqual<HRESULT>(E_NOTIMPL, hr, _T("SetCalibrationFile failed to return correct error code."));

      hr = ijsANR.SetCalibrationFile(pszSupportedFile);
      Assert::AreEqual<HRESULT>(S_OK, hr, _T("SetCalibrationFile shoulkd accept file."));
    }

    // ================================================================================================================
    TEST_METHOD(Test_DoAction)
    {
      LPCTSTR pszSineFile = _T("../../images/whale_sine.png");
      LPCTSTR pszCalibrationFile = _T("../../images/calibration.png");

      HRESULT hr;

      CIjsFilterAdditiveNoiseRemoval ijsANR;

      hr = ijsANR.SetCalibrationFile(pszCalibrationFile);
      Assert::AreEqual<HRESULT>(S_OK, hr, _T("SetCalibrationFile should accept file."));

      hr = ijsANR.SetInputFile(pszSineFile);
      Assert::AreEqual<HRESULT>(S_OK, hr, _T("SetInputFile should accept file."));

      ULONG firstX;
      ULONG lastX;
      ULONG firstY;
      ULONG lastY;

      ijsANR.GetIterationLimits(firstX, lastX, firstY, lastY);

      Assert::AreEqual<ULONG>(0, firstX, _T("Incorrect firstX"));
      Assert::AreEqual<ULONG>(ijsANR.UT_GetInputImageWidth() - 1, lastX, _T("Incorrect lastX"));
      Assert::AreEqual<ULONG>(0, firstY, _T("Incorrect firstY"));
      Assert::AreEqual<ULONG>(ijsANR.UT_GetInputImageHeight() - 1, lastY, _T("Incorrect lastY"));

      ULONG pixel = 10;
      Assert::AreNotEqual<BYTE>(BASE_NOISE, ijsANR.UT_GetCalibrationValue(pixel, pixel),
        _T("Calibration value should not be equal to base noise with calibration image loaded."));
    }

	};
}
