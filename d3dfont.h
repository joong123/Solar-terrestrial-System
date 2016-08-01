#ifndef D3D_FOUNT_HEAD_FILE
#define D3D_FOUNT_HEAD_FILE
 
#include "D3DSprite.h"
#include "GameEngineHead.h"
 
//////////////////////////////////////////////////////////////////////////////////
 
//��������
struct tagD3DFontItem
{
 
//�ַ���Ϣ
UINTuChar;//�������
CD3DSpriteD3DSprite;//���徫��
 
//���λ��
CSizeSizeCell;//��Ԫ��С
CSizeSizeFont;//�����С
CPointPointOrigin;//ԭ��λ��
 
};
 
//���Ͷ���
typedef CWHArray<tagD3DFontItem *> CD3DFontItemPtrArray;
typedef CMap<UINT,UINT,tagD3DFontItem *,tagD3DFontItem *> CD3DFontItemPtrMap;
 
//////////////////////////////////////////////////////////////////////////////////
 
//�������
class GAME_ENGINE_CLASS CD3DFont
{
 
//���ñ���
protected:
UINTm_uGrayLevel;//�Ҷȵȼ�
UINTm_uMaxBufferCount;//������Ŀ
 
//�������
protected:
CFontm_Font;//�������
TEXTMETRICm_TextMetric;//��������
 
//�ں˶���
protected:
CD3DFontItemPtrMapm_FontItemPtrMap;//��������
CD3DFontItemPtrArraym_FontItemPtrActive;//��������
 
//��������
public:
//���캯��
CD3DFont();
//��������
virtual ~CD3DFont();
 
//������Ŀ
public:
//������Ŀ
UINT GetMaxBufferCount() {
 return m_uMaxBufferCount; 
}
//������Ŀ
VOID SetMaxBufferCount(UINT uMaxBufferCount) {
 m_uMaxBufferCount=uMaxBufferCount; 
}
 
//��������
public:
//ɾ������
bool DeleteFont();
//��������
bool CreateFont(LOGFONT & LogFont, UINT uGrayLevel);
//��������
bool CreateFont(INT nPointSize, LPCTSTR lpszFaceName, UINT uGrayLevel);
 
//�������
public:
//�������
bool DrawText(CD3DDevice * pD3DDevice, LPCTSTR pszString, CRect rcDraw, UINT uFormat, D3DCOLOR D3DColor);
//�������
bool DrawText(CD3DDevice * pD3DDevice, LPCTSTR pszString, INT nXPos, INT nYPos, UINT uFormat, D3DCOLOR D3DColor);
 
//�������
private:
//��������
tagD3DFontItem * ActiveFontItem(CD3DDevice * pD3DDevice, UINT uChar);
//����λ��
CSize CalePostion(CD3DDevice * pD3DDevice, LPCTSTR pszString, UINT uFormat, INT nAreaWidth);
 
};
 
//////////////////////////////////////////////////////////////////////////////////
 
#endif