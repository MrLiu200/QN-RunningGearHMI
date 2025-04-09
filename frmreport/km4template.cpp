#include "km4template.h"
#include <QDate>
#include <QTextTable>
KM4Template::KM4Template(QObject *parent) : QObject(parent)
{

}

void KM4Template::insertHeader(QTextCursor &cursor)
{
    QTextBlockFormat headerFormat;
    headerFormat.setAlignment(Qt::AlignRight); // 右对齐
    headerFormat.setBottomMargin(10); // 设置页眉与正文的间距

    QTextCharFormat headerTextFormat;
    headerTextFormat.setFontPointSize(12);
    headerTextFormat.setFontWeight(QFont::Bold);

    // 插入页眉文本
    cursor.setBlockFormat(headerFormat);
    cursor.insertText("检测报告 ● 上海诠诺物联网技术有限公司", headerTextFormat);
    cursor.insertBlock();
    // 插入分割线
    //    QTextBlockFormat lineFormat;
    //    lineFormat.setBottomMargin(10); // 横线与正文之间的间距
    //    cursor.setBlockFormat(lineFormat);

    QTextCharFormat lineCharFormat;
    lineCharFormat.setFontPointSize(2); // 设置字体大小极小，使文字不可见
    lineCharFormat.setFontWeight(QFont::Bold);

    cursor.insertText("──────────────────────────────────────────────────────────────────────────────", lineCharFormat);
    cursor.insertBlock(); // 换行，避免正文紧贴横线
}

void KM4Template::insertParagraph(QTextCursor &cursor, QString dropName)
{
    QTextBlockFormat leftIndentFormat;
    leftIndentFormat.setAlignment(Qt::AlignLeft); // 右对齐
    leftIndentFormat.setBottomMargin(10); // 设置页眉与正文的间距
    leftIndentFormat.setLeftMargin(0);     //设置与左边距的距离

    QTextCharFormat headerTextFormat;
    headerTextFormat.setFontPointSize(15);
    headerTextFormat.setFontWeight(QFont::Bold);

    cursor.setBlockFormat(leftIndentFormat);
    cursor.insertText(dropName, headerTextFormat);
    cursor.insertBlock();
}

void KM4Template::DrawCover(QTextCursor &cursor, QString Device)
{
    insertHeader(cursor);
    // 居中格式（用于标题）
    QTextBlockFormat centerFormat;
    centerFormat.setAlignment(Qt::AlignCenter);

    // 左缩进格式（用于正文）
    QTextBlockFormat leftIndentFormat;
    leftIndentFormat.setAlignment(Qt::AlignLeft);
    leftIndentFormat.setLeftMargin(90);  // 增加左侧边距，使正文不完全靠左

    // 字体样式
    QTextCharFormat titleFormat;
    titleFormat.setFontPointSize(36);
    titleFormat.setFontWeight(QFont::Bold);

    QTextCharFormat textFormat;
    textFormat.setFontPointSize(16);
    textFormat.setFontWeight(QFont::Normal);

    // 插入空行以增加间距
    for (int i = 0; i < 15; i++) {
        cursor.insertBlock();
    }

    // 插入标题，居中
    cursor.setBlockFormat(centerFormat);
    cursor.insertText("检 测 报 告\n\n", titleFormat);

    // 插入正文，增加左边距，使其不完全靠左
    cursor.setBlockFormat(leftIndentFormat);
    cursor.insertText("受 检 单 位: 云南京建轨道交通投资建设有限公司\n\n", textFormat);
    cursor.insertText(QString("受 检 样 品: %1\n\n").arg(Device), textFormat);
    cursor.insertText("检 验 类 型: 振动测试\n\n", textFormat);
    cursor.insertText("报 告 日 期: " + QDate::currentDate().toString("yyyy-MM-dd") + "\n\n\n\n", textFormat);

    cursor.setBlockFormat(centerFormat);
    cursor.insertText("上海诠诺物联网技术有限公司\n\n", textFormat);

    // 添加分页符，准备转到下一页
    QTextBlockFormat pageBreakFormat;
    pageBreakFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
    cursor.setBlockFormat(pageBreakFormat);
    cursor.insertBlock(pageBreakFormat);
}

void KM4Template::DrawNotice(QTextCursor &cursor)
{
    insertHeader(cursor);
    // 居中格式（用于标题）
    QTextBlockFormat centerFormat;
    centerFormat.setAlignment(Qt::AlignCenter);

    // 字体样式
    QTextCharFormat titleFormat;
    titleFormat.setFontPointSize(36);
    titleFormat.setFontWeight(QFont::Bold);

    // 插入标题，居中
    cursor.setBlockFormat(centerFormat);
    cursor.insertText("注 意 事 项\n\n", titleFormat);

    // 左缩进格式（用于正文）
    QTextBlockFormat leftIndentFormat;
    leftIndentFormat.setAlignment(Qt::AlignLeft);
    leftIndentFormat.setLeftMargin(10);  // 增加左侧边距，使正文不完全靠左

    QTextCharFormat textFormat;
    textFormat.setFontPointSize(12);
    textFormat.setFontWeight(QFont::Normal);

    //插入正文
    cursor.setBlockFormat(leftIndentFormat);
    cursor.insertText("1、本报告无“上海诠诺物联网技术有限公司专用章”无效。\n\n", textFormat);
    cursor.insertText("2、本报告涂改无效。\n\n", textFormat);
    cursor.insertText("3、未经本公司书面许可，部分复制、摘用或篡改本报告内容，引起法律纠纷，责任自负\n\n", textFormat);
    cursor.insertText("4、检测分析结果仅对受检样品有效。\n\n", textFormat);
    cursor.insertText("5、对检测报告若有异议，请于收到报告之日起一个月内向本公司提出，逾期不再受理。\n\n", textFormat);

    // 插入空行以增加间距
    for (int i = 0; i < 15; i++) {
        cursor.insertBlock();
    }

    //其他
    QTextCharFormat otherFormat;
    otherFormat.setFontPointSize(10.5);
    otherFormat.setFontWeight(QFont::Normal);
    cursor.insertText("地址：上海市宝山区园丰路69号2幢302室\n\n", otherFormat);
    cursor.insertText("电话：021-59568663\n\n", otherFormat);

    // 添加分页符，准备转到下一页
    QTextBlockFormat pageBreakFormat;
    pageBreakFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
    cursor.setBlockFormat(pageBreakFormat);
    cursor.insertBlock(pageBreakFormat);

}

void KM4Template::DrawTableInfo(QTextCursor &cursor, QString Device, QString StartDate, QString EndData)
{
    insertHeader(cursor);
    cursor.insertBlock();


    //表格设置
    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignCenter);
    tableFormat.setBorder(1);
    tableFormat.setCellSpacing(2);
    tableFormat.setCellPadding(4);

    // 设置列宽
    tableFormat.setColumnWidthConstraints({
                                              QTextLength(QTextLength::PercentageLength, 30), // 第1列宽度为50%
                                              QTextLength(QTextLength::PercentageLength, 70)  // 第2列宽度为50%
                                          });

    QTextTable *table = cursor.insertTable(9, 2, tableFormat);
    QTextCharFormat tableTextFormat;
    tableTextFormat.setFontPointSize(16);


    // 填充表格内容
    table->cellAt(0, 0).firstCursorPosition().insertText("检 测 设 备", tableTextFormat);
    table->cellAt(0, 1).firstCursorPosition().insertText("轨交走行部", tableTextFormat);

    table->cellAt(1, 0).firstCursorPosition().insertText("列 车 编 号", tableTextFormat);
    table->cellAt(1, 1).firstCursorPosition().insertText(QString("%1").arg(Device), tableTextFormat);

    table->cellAt(2, 0).firstCursorPosition().insertText("监 测 设 备", tableTextFormat);
    table->cellAt(2, 1).firstCursorPosition().insertText("QN-RDFDS-KM4走行部在线故障监测系统", tableTextFormat);

    table->cellAt(3, 0).firstCursorPosition().insertText("使 用 单 位", tableTextFormat);
    table->cellAt(3, 1).firstCursorPosition().insertText("云南京建轨道交通投资建设有限公司", tableTextFormat);

    table->cellAt(4, 0).firstCursorPosition().insertText("车 厢 数 量", tableTextFormat);
    table->cellAt(4, 1).firstCursorPosition().insertText(QString("%1").arg(WagonList.size()), tableTextFormat);

    table->cellAt(5, 0).firstCursorPosition().insertText("轴 位 编 号", tableTextFormat);
    table->cellAt(5, 1).firstCursorPosition().insertText(QString("1#-%1#").arg(resultdata.size()), tableTextFormat);

    table->cellAt(6, 0).firstCursorPosition().insertText("起 始 日 期", tableTextFormat);
    table->cellAt(6, 1).firstCursorPosition().insertText(QString("%1").arg(StartDate), tableTextFormat);

    table->cellAt(7, 0).firstCursorPosition().insertText("截 止 日 期", tableTextFormat);
    table->cellAt(7, 1).firstCursorPosition().insertText(QString("%1").arg(EndData), tableTextFormat);

    table->cellAt(8, 0).firstCursorPosition().insertText("报 告 日 期", tableTextFormat);
    table->cellAt(8, 1).firstCursorPosition().insertText(QDate::currentDate().toString("yyyy/MM/dd"), tableTextFormat);


    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 9); // 移动9行
    cursor.movePosition(QTextCursor::NextBlock);

    // 添加分页符，准备转到下一页
    QTextBlockFormat pageBreakFormat;
    pageBreakFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
    cursor.setBlockFormat(pageBreakFormat);
    cursor.insertBlock();
}

void KM4Template::DrawResultOverview(QTextCursor &cursor, QTextDocument *doc, QString Device)
{
    //正文
    insertHeader(cursor);
    // 居中格式
    QTextBlockFormat centerFormat;
    centerFormat.setAlignment(Qt::AlignCenter);

    // 字体样式
    QTextCharFormat titleFormat;
    titleFormat.setFontPointSize(14);
    titleFormat.setFontWeight(QFont::Bold);

    // 插入标题，居中
    cursor.setBlockFormat(centerFormat);
    cursor.insertText("诊 断 结 果\n\n", titleFormat);

    //插入段落名称
    insertParagraph(cursor,"1、概述");


    // 左缩进格式（用于正文）
    QTextBlockFormat leftIndentFormat;
    leftIndentFormat.setAlignment(Qt::AlignLeft);
    leftIndentFormat.setLeftMargin(10);  // 增加左侧边距，使正文不完全靠左

    QTextCharFormat textFormat;
    textFormat.setFontPointSize(10.5);
    textFormat.setFontWeight(QFont::Normal);

    //插入正文
    cursor.setBlockFormat(leftIndentFormat);
    cursor.insertText(QString("本报告基于QN-RDFDS-KM4走行部在线故障监测系统对%1 – %2时间段内的数据进行振动分析及运行状态评估。\n\n").arg(StartTime).arg(EndTime), textFormat);

    cursor.insertBlock();

    //插入图片
    QTextImageFormat imageFormat;

    imageFormat.setName(":/image/image/report/KM4Report_1.png");
    imageFormat.setHeight(300);
    imageFormat.setWidth(doc->pageSize().width()); // 让图片宽度占满100%

    cursor.insertImage(imageFormat);
    cursor.insertBlock();
    cursor.insertText("\n");
    imageFormat.setName(":/image/image/report/KM4Report_2.png");
    cursor.insertImage(imageFormat);

    cursor.insertBlock();
    //插入表格
    QTextBlockFormat tableTitleFormat;
    tableTitleFormat.setAlignment(Qt::AlignCenter);

    QTextCharFormat tabletextFormat;
    tabletextFormat.setFontPointSize(10.5);
    tabletextFormat.setFontWeight(QFont::Normal);


    cursor.setBlockFormat(tableTitleFormat);
    cursor.insertText(QString("%1测点配置示意图及测点说明\n\n").arg(Device), tabletextFormat);

    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignCenter);
    tableFormat.setBorder(1);
    tableFormat.setCellSpacing(2);
    tableFormat.setCellPadding(4);

    // 设置列宽
    tableFormat.setColumnWidthConstraints({
                                              QTextLength(QTextLength::PercentageLength, 20),
                                              QTextLength(QTextLength::PercentageLength, 40),
                                              QTextLength(QTextLength::PercentageLength, 50)
                                          });

    QTextTable *table = cursor.insertTable(6, 3, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText("序号", tabletextFormat);
    table->cellAt(0, 1).firstCursorPosition().insertText("测点名称", tabletextFormat);
    table->cellAt(0, 2).firstCursorPosition().insertText("传感器类型", tabletextFormat);

    table->cellAt(1, 0).firstCursorPosition().insertText("1", tabletextFormat);
    table->cellAt(1, 1).firstCursorPosition().insertText("1侧轴箱径向", tabletextFormat);
    table->cellAt(1, 2).firstCursorPosition().insertText("通频复合加速度传感器", tabletextFormat);

    table->cellAt(2, 0).firstCursorPosition().insertText("2", tabletextFormat);
    table->cellAt(2, 1).firstCursorPosition().insertText("2侧轴箱径向", tabletextFormat);
    table->cellAt(2, 2).firstCursorPosition().insertText("通频复合加速度传感器", tabletextFormat);

    table->cellAt(3, 0).firstCursorPosition().insertText("3", tabletextFormat);
    table->cellAt(3, 1).firstCursorPosition().insertText("齿轮箱径向", tabletextFormat);
    table->cellAt(3, 2).firstCursorPosition().insertText("通频复合加速度传感器", tabletextFormat);

    table->cellAt(4, 0).firstCursorPosition().insertText("4", tabletextFormat);
    table->cellAt(4, 1).firstCursorPosition().insertText("电机传动端径向", tabletextFormat);
    table->cellAt(4, 2).firstCursorPosition().insertText("通频复合加速度传感器", tabletextFormat);

    table->cellAt(5, 0).firstCursorPosition().insertText("5", tabletextFormat);
    table->cellAt(5, 1).firstCursorPosition().insertText("电机非传动端径向", tabletextFormat);
    table->cellAt(5, 2).firstCursorPosition().insertText("通频复合加速度传感器", tabletextFormat);

    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, 6);
    cursor.movePosition(QTextCursor::NextBlock);

    // 添加分页符，准备转到下一页
    QTextBlockFormat pageBreakFormat;
    pageBreakFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
    cursor.setBlockFormat(pageBreakFormat);
    cursor.insertBlock();
}

void KM4Template::DrawResultTable(QTextCursor &cursor, const QVector<QVector<QString> > &ResultData)
{
    insertHeader(cursor);
    insertParagraph(cursor,"2、运行状态");
    QTextBlockFormat leftIndentFormat;
    leftIndentFormat.setAlignment(Qt::AlignLeft);
    leftIndentFormat.setLeftMargin(10);  // 增加左侧边距，使正文不完全靠左

    QTextCharFormat textFormat1;
    textFormat1.setFontPointSize(10.5);
    textFormat1.setFontWeight(QFont::Normal);

    //插入正文
    cursor.setBlockFormat(leftIndentFormat);
    cursor.insertText("机组运行状况是结合现场运行状况进行频域特征分析，最终得出机组健康状态。如下表所示：\n\n", textFormat1);

    //    cursor.insertBlock();

    // 设置表格格式
    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignCenter);
    tableFormat.setBorder(1);
    tableFormat.setCellSpacing(2);
    tableFormat.setCellPadding(4);

    // 设置列宽比例
    tableFormat.setColumnWidthConstraints({
                                              QTextLength(QTextLength::PercentageLength, 15), // 车厢号
                                              QTextLength(QTextLength::PercentageLength, 15), // 轴位编号
                                              QTextLength(QTextLength::PercentageLength, 14), // 1侧轴箱
                                              QTextLength(QTextLength::PercentageLength, 14), // 2侧轴箱
                                              QTextLength(QTextLength::PercentageLength, 14), // 齿轮箱
                                              QTextLength(QTextLength::PercentageLength, 14), // 电机传动端
                                              QTextLength(QTextLength::PercentageLength, 14)  // 电机非传动端
                                          });

    // 创建表格
    QTextTable *table = cursor.insertTable(ResultData.size()+1, 7, tableFormat);

    // 居中格式
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignCenter);

    QTextCharFormat textFormat;
    textFormat.setFontPointSize(12);

    // 表头
    QStringList headers = {"车厢号", "轴位编号", "1侧轴箱", "2侧轴箱", "齿轮箱", "电机传动端", "电机非传动端"};
    for (int col = 0; col < 7; ++col) {
        QTextCursor cellCursor = table->cellAt(0, col).firstCursorPosition();
        cellCursor.setBlockFormat(blockFormat);
        cellCursor.insertText(headers[col], textFormat);
    }

    QStringList wagonlist;
    int wagoncount = WagonList.size();
    for(int i=0;i<wagoncount;i++){
        wagonlist.append(WagonList.at(i));
        wagonlist.append("");
        wagonlist.append("");
        wagonlist.append("");
    }



    QStringList AxisNumberList;
    for(int i=0;i<ResultData.size();i++){
        AxisNumberList.append(QString("%1#").arg(i+1));
    }


   QVector<RowData> data;
   for(int i=0;i<ResultData.size();i++){
       auto Result = ResultData.at(i);
       RowData onecountdata = createRowData(wagonlist.at(i),AxisNumberList.at(i),
                                            Result.at(0),Result.at(1),Result.at(2),Result.at(3),Result.at(4));
       data.append(onecountdata);
   }

    // 插入数据
    int rowIndex = 1;
    for (const auto &row : data) {
        // 车厢号合并（相同车厢号的单元格合并）
        if (!row.carNumber.isEmpty()) {
            int mergeCount = 1;
            while (rowIndex + mergeCount < data.size() && data[rowIndex + mergeCount].carNumber.isEmpty()) {
                ++mergeCount;
            }
            table->mergeCells(rowIndex, 0, 4, 1);
            QTextCursor carCursor = table->cellAt(rowIndex, 0).firstCursorPosition();
            carCursor.setBlockFormat(blockFormat);
            carCursor.insertText(row.carNumber, textFormat);
        }

        // 轴位编号
        QTextCursor axisCursor = table->cellAt(rowIndex, 1).firstCursorPosition();
        axisCursor.setBlockFormat(blockFormat);
        axisCursor.insertText(row.axisNumber, textFormat);

        // 其他列数据
        QVector<CellData> values = {row.side1, row.side2, row.gearbox, row.motorDrive, row.motorNonDrive};
        for (int col = 0; col < values.size(); ++col) {
            QTextCursor cellCursor = table->cellAt(rowIndex, col + 2).firstCursorPosition();
            cellCursor.setBlockFormat(blockFormat);
            cellCursor.insertText(values[col].value, textFormat);

            // 设置背景颜色
            QTextCharFormat bgFormat;
            bgFormat.setBackground(values[col].color);
            table->cellAt(rowIndex, col + 2).setFormat(bgFormat);
        }

        ++rowIndex;
    }

    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, ResultData.size()+1);
    cursor.movePosition(QTextCursor::NextBlock);
    //注释
    cursor.setBlockFormat(leftIndentFormat);
    textFormat1.setFontWeight(QFont::Bold);
    cursor.insertText("故障等级说明\n\n", textFormat1);

    textFormat1.setFontWeight(QFont::Normal);
    cursor.insertText("正常：机组处于正常运行状态；\n\n", textFormat1);
    cursor.insertText("预警：机组处于正常运行状态；机组存在早期故障特征频率，通过日常巡检维护，可满足机组正常运行条件；\n\n", textFormat1);
    cursor.insertText("警告：机组故障特征频率较明显，故障处于发展期，现场运维工程师需在1周内检查故障，择机维护、维修；\n\n", textFormat1);
    cursor.insertText("报警：机组故障特征频率明显，故障处于劣化期，现场运维工程师需立即检查故障，择机维修、更换；\n\n", textFormat1);
    cursor.insertText("无：无该测点；\n\n", textFormat1);
    cursor.insertText("离线：测点处于离线状态；", textFormat1);

    // 添加分页符，准备转到下一页
    QTextBlockFormat pageBreakFormat;
    pageBreakFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
    cursor.setBlockFormat(pageBreakFormat);
    cursor.insertBlock();
}

void KM4Template::DrawResultSuggestions(QTextCursor &cursor, const QVector<QVector<QString> > &ResultData)
{
    insertHeader(cursor);
    insertParagraph(cursor,"3、诊断结论及处理建议");

    // 居中格式
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignCenter);

    QTextCharFormat textFormat;
    textFormat.setFontPointSize(10.5);

    QTextCharFormat boldFormat;
    boldFormat.setFontPointSize(10.5);
    boldFormat.setFontWeight(QFont::Bold);

    cursor.setBlockFormat(blockFormat);
    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignCenter);
    tableFormat.setBorder(1);
    tableFormat.setCellSpacing(2);
    tableFormat.setCellPadding(4);

    // 设置列宽比例
    tableFormat.setColumnWidthConstraints({
                                              QTextLength(QTextLength::PercentageLength, 30),
                                              QTextLength(QTextLength::PercentageLength, 70),
                                          });

    // 创建表格
    QTextTable *table = cursor.insertTable(ResultData.size()+1, 2, tableFormat);

    //填充表头
    QStringList headTextList;
    headTextList << "轴位编号" << "测试结论及处理建议";
    for(int i=0;i<headTextList.size();i++){
        QTextCursor cellCursor = table->cellAt(0, i).firstCursorPosition();
        cellCursor.setBlockFormat(blockFormat);
        cellCursor.insertText(headTextList.at(i), textFormat);
    }

    //第一列为序号，先构建一个数组，填充第一列
    QStringList AxisNumberList;
    for(int i=0;i<ResultData.size();i++){
        AxisNumberList.append(QString("%1#").arg(i+1));
    }
    for(int i=0;i<AxisNumberList.size();i++){
        QTextCursor AxisCursor = table->cellAt(i+1, 0).firstCursorPosition();
        AxisCursor.setBlockFormat(blockFormat);
        AxisCursor.insertText(AxisNumberList.at(i), textFormat);
    }


    //获取第二列内容
    QStringList evaluationResults;
    QStringList headers = {"1侧轴箱", "2侧轴箱", "齿轮箱", "电机传动端", "电机非传动端"};
    for(auto Result : ResultData){
        QString str = getHandlingSuggestions(Result,headers);
        evaluationResults.append(str);
    }

    // 第二列（测试结论及处理建议）
    for (int row = 0; row < evaluationResults.size(); ++row) {
        QTextCursor cellCursor = table->cellAt(row+1, 1).firstCursorPosition();

        QStringList lines = evaluationResults[row].split("\n");
        for (const QString &line : lines) {
            if (line.startsWith("▶")) {  // 标题部分加粗
                cellCursor.insertText(line + "\n", boldFormat);
            } else {  // 普通文本
                cellCursor.insertText(line + "\n", textFormat);
            }
        }
    }

    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, ResultData.size()+1);
    cursor.movePosition(QTextCursor::NextBlock);

    // 添加分页符，准备转到下一页
    QTextBlockFormat pageBreakFormat;
    pageBreakFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysAfter);
    cursor.setBlockFormat(pageBreakFormat);
    cursor.insertBlock();
}

void KM4Template::DrawDisclaimers(QTextCursor &cursor)
{
    insertHeader(cursor);
    cursor.insertBlock();
    insertParagraph(cursor,"4、免责声明");

    QTextBlockFormat leftIndentFormat;
    leftIndentFormat.setAlignment(Qt::AlignLeft);
    leftIndentFormat.setLeftMargin(10);  // 增加左侧边距，使正文不完全靠左

    QTextCharFormat textFormat1;
    textFormat1.setFontPointSize(10.5);
    textFormat1.setFontWeight(QFont::Normal);

    //插入正文
    cursor.setBlockFormat(leftIndentFormat);
    cursor.insertText("1、上海诠诺物联网技术有限公司仅作为客户在线监测系统安装、"
                      "调试及走行部监测设备的维护、服务咨询等相关工作，并根据客户需求对监测机组提供参考性的故障诊断处理意见\n\n", textFormat1);
    cursor.insertText("2、针对走行部机组机械和控制系统所采取的任何措施，如停机、维修、更换均由客户自己决定。\n\n", textFormat1);
    cursor.insertText("3、上海诠诺物联网技术有限公司不承担因机组生产中断或其它原因引起的任何损失、损坏和人员伤害。\n\n", textFormat1);
    cursor.insertText("4、如签订技术协议，以技术协议双方职责为准。\n\n", textFormat1);
}

QColor KM4Template::getStatusColor(const QString &status)
{
    if (status == "正常") return Qt::green;
    if (status == "预警") return Qt::cyan;
    if (status == "警告") return Qt::yellow;
    if (status == "报警") return Qt::red;
    if (status == "离线") return Qt::gray;
    if (status == "无") return Qt::white;
    return Qt::black;  // 默认颜色
}

KM4Template::RowData KM4Template::createRowData(const QString &carNumber, const QString &axisNumber, const QString &side1Status,
                                                const QString &side2Status, const QString &gearboxStatus, const QString &motorDriveStatus,
                                                const QString &motorNonDriveStatus)
{
    return {
        carNumber, axisNumber,
        {side1Status, getStatusColor(side1Status)},
        {side2Status, getStatusColor(side2Status)},
        {gearboxStatus, getStatusColor(gearboxStatus)},
        {motorDriveStatus, getStatusColor(motorDriveStatus)},
        {motorNonDriveStatus, getStatusColor(motorNonDriveStatus)}
    };
}

QString KM4Template::getHandlingSuggestions(QVector<QString> Result, QStringList HeadList)
{
    if(Result.size() != HeadList.size()){
        QString str = "▶ 评价结果\n无。\n\n"
                      "▶ 处理意见\n无。";
        return str;
    }
    QStringList retlist;
    retlist.append("▶ 评价结果\n");
    QStringList suggestionlist;
    suggestionlist.append("▶ 处理意见\n");
    int counter = 0;
    int errorcount = 1;
    for(int i=0;i<Result.size();i++){
        if(Result[i] == "正常"){
            counter += 1;
        }else if(Result[i] == "离线"){
            QString Hand = HeadList.at(i);
            retlist.append(QString("(%1) ").arg(errorcount) + QString("%1测点处于离线状态。\n").arg(Hand));
            suggestionlist.append(QString("(%1) 检查设备运行情况或联系我司排查处理。\n").arg(errorcount));
            errorcount++;
        }else{
            QString Hand = HeadList.at(i);
            if(Hand.contains("轴箱")){
                retlist.append(QString("(%1) ").arg(errorcount) + Hand + "轴承故障\n");
                suggestionlist.append(QString("(%1) 建议检查%2\n").arg(errorcount).arg(Hand));
                errorcount++;
            }else if(Hand.contains("齿轮箱")){
                retlist.append(QString("(%1) ").arg(errorcount) + Hand + "齿隙故障\n");
                suggestionlist.append(QString("(%1) 建议检查%2\n").arg(errorcount).arg(Hand));
                errorcount++;
            }else if(Hand.contains("电机")){
                retlist.append(QString("(%1) ").arg(errorcount) + Hand + "轴承故障\n");
                suggestionlist.append(QString("(%1) 建议检查%2\n").arg(errorcount).arg(Hand));
                errorcount++;
            }
        }
    }
    if(counter == Result.size()){
        retlist.append("运行部件传动链运行正常。");
        suggestionlist.append("无");
    }
    QString retstr = retlist.join("") + suggestionlist.join("");
    return retstr;
}

void KM4Template::SetData(QString carnumber, QString StartDate, QString EndData, QString savefile, QStringList wagonlist, QList<ReportHelp::PDFDEVICE> devicelist)
{
    this->CarNum = carnumber;
    this->StartTime = StartDate;
    this->EndTime = EndData;
    this->WagonList = wagonlist;
    this->SavePath = savefile;
    this->DeviceInfoList = devicelist;
    //将数据内容进行分析
    //创建一个队列存储信息

    resultdata.clear();
    // 告警等级转字符串
    auto getAlarmStr = [](int grade) -> QString {
        switch (grade) {
        case -1: return "离线";
        case 0: return "预警";
        case 1: return "警告";
        case 2: return "报警";
        default: return "正常";
        }
    };

    // 获取轴位状态（5种设备）
    auto getAxisStatus = [&](const QList<int>& indices) -> QVector<QString> {
        QVector<QString> status(5, "正常");
        for (int idx : indices) {
            const auto& device = DeviceInfoList[idx];
            QString alarm = getAlarmStr(device.alarmgrade);

            if (device.type == "1侧轴箱") status[0] = alarm;
            else if (device.type == "2侧轴箱") status[1] = alarm;
            else if (device.type == "齿轮箱") status[2] = alarm;
            else if (device.type == "电机传动端") status[3] = alarm;
            else if (device.type == "电机非传动端") status[4] = alarm;
        }
        return status;
    };

    for (const QString& wagonname : WagonList) {
        QList<int> axisIndices[4]; // AxisPosition 1~4

        for (int i = 0; i < DeviceInfoList.size(); ++i) {
            const auto& device = DeviceInfoList[i];
            if (device.Wagon == wagonname && device.AxisPosition >= 1 && device.AxisPosition <= 4) {
                axisIndices[device.AxisPosition - 1].append(i);
            }
        }

        // 添加4个轴位状态
        for (int i = 0; i < 4; ++i) {
            resultdata.append(getAxisStatus(axisIndices[i]));
        }
    }
}

bool KM4Template::drawReport()
{
    // 创建打印机对象
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setOutputFileName(SavePath);

    // 创建文档对象
    QTextDocument *doc = new QTextDocument();
    doc->setPageSize(printer.pageRect().size());
    QTextCursor cursor(doc);

    DrawCover(cursor,"KM4-"+CarNum);
    doc->print(&printer);

    DrawNotice(cursor);
    doc->print(&printer);

    DrawTableInfo(cursor,"KM4-"+CarNum,StartTime,EndTime);
    doc->print(&printer);

    DrawResultOverview(cursor, doc,"KM4-T35");
    doc->print(&printer);


    DrawResultTable(cursor,resultdata);
    doc->print(&printer);

    DrawResultSuggestions(cursor,resultdata);
    doc->print(&printer);

    DrawDisclaimers(cursor);
    doc->print(&printer);

    doc->end();

    return true;
}
