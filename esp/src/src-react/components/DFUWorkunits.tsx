import * as React from "react";
import { CommandBar, ContextualMenuItemType, ICommandBarItemProps, Icon, Image, Link } from "@fluentui/react";
import { SizeMe } from "../layouts/SizeMe";
import * as ESPDFUWorkunit from "src/ESPDFUWorkunit";
import * as FileSpray from "src/FileSpray";
import * as Utility from "src/Utility";
import { QuerySortItem } from "src/store/Store";
import nlsHPCC from "src/nlsHPCC";
import { useConfirm } from "../hooks/confirm";
import { useMyAccount } from "../hooks/user";
import { HolyGrail } from "../layouts/HolyGrail";
import { pushParams } from "../util/history";
import { FluentPagedGrid, FluentPagedFooter, useCopyButtons, useFluentStoreState, FluentColumns } from "./controls/Grid";
import { Filter } from "./forms/Filter";
import { Fields } from "./forms/Fields";
import { ShortVerticalDivider } from "./Common";
import { selector } from "./DojoGrid";
import { SashaService, WsSasha } from "@hpcc-js/comms";
import { scopedLogger } from "@hpcc-js/util";

const logger = scopedLogger("src-react/components/DFUWorkunits.tsx");

const FilterFields: Fields = {
    "Type": { type: "checkbox", label: nlsHPCC.ArchivedOnly },
    "Wuid": { type: "string", label: nlsHPCC.WUID, placeholder: "D20201203-171723" },
    "Owner": { type: "string", label: nlsHPCC.Owner, placeholder: nlsHPCC.jsmi },
    "Jobname": { type: "string", label: nlsHPCC.Jobname, placeholder: nlsHPCC.log_analysis_1 },
    "Cluster": { type: "target-cluster", label: nlsHPCC.Cluster, placeholder: nlsHPCC.Owner },
    "StateReq": { type: "dfuworkunit-state", label: nlsHPCC.State, placeholder: nlsHPCC.Created },
};

function formatQuery(_filter): { [id: string]: any } {
    const filter = { ..._filter };
    if (filter.StartDate) {
        filter.StartDate = new Date(filter.StartDate).toISOString();
    }
    if (filter.EndDate) {
        filter.EndDate = new Date(filter.StartDate).toISOString();
    }
    if (filter.Type === true) {
        filter.Type = "archived workunits";
    }
    if (filter.Type === true) {
        filter.Type = "archived workunits";
    }
    return filter;
}

const defaultUIState = {
    hasSelection: false,
    hasProtected: false,
    hasNotProtected: false,
    hasFailed: false,
    hasNotFailed: false
};

interface DFUWorkunitsProps {
    filter?: { [id: string]: any };
    sort?: QuerySortItem;
    store?: any;
    page?: number;
}

const emptyFilter = {};
const defaultSort = { attribute: "Wuid", descending: true };

export const DFUWorkunits: React.FunctionComponent<DFUWorkunitsProps> = ({
    filter = emptyFilter,
    sort = defaultSort,
    store,
    page
}) => {

    const hasFilter = React.useMemo(() => Object.keys(filter).length > 0, [filter]);

    const [showFilter, setShowFilter] = React.useState(false);
    const sashaService = React.useMemo(() => new SashaService({ baseUrl: "" }), []);
    const { currentUser } = useMyAccount();
    const [uiState, setUIState] = React.useState({ ...defaultUIState });
    const {
        selection, setSelection,
        pageNum, setPageNum,
        pageSize, setPageSize,
        total, setTotal,
        refreshTable } = useFluentStoreState({ page });

    //  Grid ---
    const gridStore = React.useMemo(() => {
        return store || ESPDFUWorkunit.CreateWUQueryStore();
    }, [store]);

    const query = React.useMemo(() => {
        return formatQuery(filter);
    }, [filter]);

    const columns = React.useMemo((): FluentColumns => {
        return {
            col1: selector({
                width: 27,
                selectorType: "checkbox"
            }),
            isProtected: {
                headerIcon: "LockSolid",
                width: 18,
                sortable: false,
                formatter: (_protected) => {
                    if (_protected === true) {
                        return <Icon iconName="LockSolid" />;
                    }
                    return "";
                }
            },
            ID: {
                label: nlsHPCC.ID,
                width: 130,
                formatter: (ID, idx) => {
                    const wu = ESPDFUWorkunit.Get(ID);
                    return <>
                        <Image src={wu.getStateImage()} styles={{ root: { minWidth: "16px" } }} />
                        &nbsp;
                        <Link href={`#/dfuworkunits/${ID}`}>{ID}</Link>
                    </>;
                }
            },
            Command: {
                label: nlsHPCC.Type,
                width: 110,
                formatter: (command) => {
                    if (command in FileSpray.CommandMessages) {
                        return FileSpray.CommandMessages[command];
                    }
                    return "Unknown";
                }
            },
            User: { label: nlsHPCC.Owner, width: 90 },
            JobName: { label: nlsHPCC.JobName, width: 220 },
            ClusterName: { label: nlsHPCC.Cluster, width: 70 },
            StateMessage: { label: nlsHPCC.State, width: 70 },
            PercentDone: {
                label: nlsHPCC.PctComplete, width: 80, sortable: true,
            },
            TimeStarted: { label: nlsHPCC.TimeStarted, width: 100, sortable: true },
            TimeStopped: { label: nlsHPCC.TimeStopped, width: 100, sortable: true },
            KbPerSec: {
                label: nlsHPCC.TransferRate, width: 90,
                formatter: (value, row) => {
                    return Utility.convertedSize(row.KbPerSec * 1024) + " / sec";
                }
            },
            KbPerSecAve: { // KbPerSecAve seems to never be different than KbPerSec, see HPCC-29894
                label: nlsHPCC.TransferRateAvg, width: 90,
                formatter: (value, row) => {
                    return Utility.convertedSize(row.KbPerSecAve * 1024) + " / sec";
                }
            }
        };
    }, []);

    const [DeleteConfirm, setShowDeleteConfirm] = useConfirm({
        title: nlsHPCC.Delete,
        message: nlsHPCC.DeleteSelectedWorkunits,
        items: selection.map(s => s.Wuid),
        onSubmit: React.useCallback(() => {
            FileSpray.DFUWorkunitsAction(selection, nlsHPCC.Delete).then(() => refreshTable.call(true));
        }, [refreshTable, selection])
    });

    //  Command Bar  ---
    const buttons = React.useMemo((): ICommandBarItemProps[] => [
        {
            key: "refresh", text: nlsHPCC.Refresh, iconProps: { iconName: "Refresh" },
            onClick: () => refreshTable.call()
        },
        { key: "divider_1", itemType: ContextualMenuItemType.Divider, onRender: () => <ShortVerticalDivider /> },
        {
            key: "open", text: nlsHPCC.Open, disabled: !uiState.hasSelection, iconProps: { iconName: "WindowEdit" },
            onClick: () => {
                if (selection.length === 1) {
                    window.location.href = `#/dfuworkunits/${selection[0].ID}`;
                } else {
                    for (let i = selection.length - 1; i >= 0; --i) {
                        window.open(`#/dfuworkunits/${selection[i].ID}`, "_blank");
                    }
                }
            }
        },
        {
            key: "delete", text: nlsHPCC.Delete, disabled: !uiState.hasNotProtected, iconProps: { iconName: "Delete" },
            onClick: () => setShowDeleteConfirm(true)
        },
        { key: "divider_2", itemType: ContextualMenuItemType.Divider, onRender: () => <ShortVerticalDivider /> },
        {
            key: "setFailed", text: nlsHPCC.SetToFailed, disabled: !uiState.hasNotProtected,
            onClick: () => { FileSpray.DFUWorkunitsAction(selection, "SetToFailed"); }
        },
        { key: "divider_3", itemType: ContextualMenuItemType.Divider, onRender: () => <ShortVerticalDivider /> },
        {
            key: "restore", text: nlsHPCC.Restore, disabled: !uiState.hasSelection,
            onClick: () => {
                const wuids = selection.map(item => item.Wuid || item.ID);
                Promise.all(wuids.map(wuid =>
                    sashaService.RestoreWU({
                        Wuid: wuid,
                        WUType: WsSasha.WUTypes.DFU
                    })
                )).then(() => {
                    refreshTable.call(true);
                }).catch(err => {
                    logger.error(err);
                });
            }
        },
        {
            key: "protect", text: nlsHPCC.Protect, disabled: !uiState.hasNotProtected,
            onClick: () => { FileSpray.DFUWorkunitsAction(selection, "Protect").then(() => refreshTable.call()); }
        },
        {
            key: "unprotect", text: nlsHPCC.Unprotect, disabled: !uiState.hasProtected,
            onClick: () => { FileSpray.DFUWorkunitsAction(selection, "Unprotect").then(() => refreshTable.call()); }
        },
        { key: "divider_4", itemType: ContextualMenuItemType.Divider, onRender: () => <ShortVerticalDivider /> },
        {
            key: "filter", text: nlsHPCC.Filter, disabled: !!store, iconProps: { iconName: hasFilter ? "FilterSolid" : "Filter" },
            onClick: () => {
                setShowFilter(true);
            }
        },
        {
            key: "mine", text: nlsHPCC.Mine, disabled: !currentUser?.username || !total, iconProps: { iconName: "Contact" }, canCheck: true, checked: filter["Owner"] === currentUser.username,
            onClick: () => {
                if (filter["Owner"] === currentUser.username) {
                    filter["Owner"] = "";
                } else {
                    filter["Owner"] = currentUser.username;
                }
                pushParams(filter);
            }
        },
    ], [currentUser, filter, hasFilter, refreshTable, sashaService, selection, setShowDeleteConfirm, store, total, uiState.hasNotProtected, uiState.hasProtected, uiState.hasSelection]);

    const copyButtons = useCopyButtons(columns, selection, "dfuworkunits");

    //  Filter  ---
    const filterFields: Fields = {};
    for (const field in FilterFields) {
        filterFields[field] = { ...FilterFields[field], value: filter[field] };
    }

    //  Selection  ---
    React.useEffect(() => {
        const state = { ...defaultUIState };

        for (let i = 0; i < selection.length; ++i) {
            state.hasSelection = true;
            if (selection[i] && selection[i].isProtected && selection[i].isProtected !== false) {
                state.hasProtected = true;
            } else {
                state.hasNotProtected = true;
            }
            if (selection[i] && selection[i].State && selection[i].State === 5) {
                state.hasFailed = true;
            } else {
                state.hasNotFailed = true;
            }
        }
        setUIState(state);
    }, [selection]);

    return <HolyGrail
        header={<CommandBar items={buttons} farItems={copyButtons} />}
        main={
            <>
                <SizeMe>{({ size }) =>
                    <div style={{ width: "100%", height: "100%" }}>
                        <div style={{ position: "absolute", width: "100%", height: `${size.height}px` }}>
                            <FluentPagedGrid
                                store={gridStore}
                                query={query}
                                sort={sort}
                                pageNum={pageNum}
                                pageSize={pageSize}
                                total={total}
                                columns={columns}
                                height={`${size.height}px`}
                                setSelection={setSelection}
                                setTotal={setTotal}
                                refresh={refreshTable}
                            ></FluentPagedGrid>
                        </div>
                    </div>
                }</SizeMe>
                <Filter showFilter={showFilter} setShowFilter={setShowFilter} filterFields={filterFields} onApply={pushParams} />
                <DeleteConfirm />
            </>
        }
        footer={<FluentPagedFooter
            persistID={"dfuworkunits"}
            pageNum={pageNum}
            selectionCount={selection.length}
            setPageNum={setPageNum}
            setPageSize={setPageSize}
            total={total}
        ></FluentPagedFooter>}
    />;
};